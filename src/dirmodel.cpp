#include <QImageReader>
#include <Kirigami/Units>
#include <KDirLister>
#include <KSharedConfig>
#include <KConfigGroup>
#include <QCache>

#include <sys/xattr.h>
#include <optional>

#include "dirmodel.h"

struct SDirModel::Private
{
	// TODO(microoptimisation): don't have a copy of this in every SDirModel
	QList<QByteArray> supportedMimeTypes;
    KSharedConfigPtr spatialState;
    KConfigGroup group;
};

SDirModel::SDirModel(QObject* parent) : KDirModel(parent), d(new Private)
{
	d->supportedMimeTypes = QImageReader::supportedMimeTypes();
    d->spatialState = KSharedConfig::openStateConfig("org.kde.melon.spatialdata");
    d->group = d->spatialState->group("Positions");
}

SDirModel::~SDirModel()
{
}

std::optional<QPoint> SDirModel::getXAttr(const KFileItem& item) const
{
    if (d->group.hasKey(item.url().toString()))
        return d->group.readEntry(item.url().toString(), QPoint());

    return {};
}

void SDirModel::setXAttr(const KFileItem& item, const QPoint& pos) const
{
    d->group.writeEntry(item.url().toString(), pos);
}

QPointF SDirModel::positionFor(const KFileItem& item) const
{
    if (auto pos = getXAttr(item)) {
        return *pos;
    }

	Kirigami::Units units;

	const auto width = units.gridUnit() * 6;
	const auto height = units.gridUnit() * 5;
	const auto overallWidth = units.gridUnit() * (40-7);
	const auto size = QSize(width, height);
	QPoint topLeft(0, 0);

    QRegion existingIcons;
    const auto items = dirLister()->items();
    for (const auto& item : items) {
		auto comparingPos = getXAttr(item);
		if (!comparingPos.has_value())
			continue;

        existingIcons += QRect(*comparingPos, size);
    }

	while (existingIcons.intersects(QRect(topLeft, size))) {
		topLeft.rx() += width;
		if (topLeft.x() + width >= overallWidth) {
			topLeft.rx() = 0;
			topLeft.ry() += height;
		}
	}

    setXAttr(item, topLeft);
    return topLeft;
}

QVariant SDirModel::data(const QModelIndex& index, int role) const
{
	auto item = itemForIndex(index);

	if (item.isNull())
		return KDirModel::data(index, role);

	switch (AdditionalRoles(role)) {
	case IsImageReadable:
		return d->supportedMimeTypes.contains(item.mimetype().toLocal8Bit());
	case IconPosition:
		return positionFor(item);
	default:
		return KDirModel::data(index, role);
	}
}

QHash<int, QByteArray> SDirModel::roleNames() const
{
	auto names = KDirModel::roleNames();
	names[AdditionalRoles::IsImageReadable] = "isImageReadable";
	names[AdditionalRoles::IconPosition] = "iconPosition";
	return names;
}
