#include <QImageReader>
#include <Kirigami/Units>
#include <KDirLister>

#include "dirmodel.h"

struct SDirModel::Private
{
	// TODO(microoptimisation): don't have a copy of this in every SDirModel
	QList<QByteArray> supportedMimeTypes;
	// TODO: write to filesystem, use more persistent ID
	QMap<QUrl, QMap<QString, QPointF>> iconPositions;
};

SDirModel::SDirModel(QObject* parent) : KDirModel(parent), d(new Private)
{
	d->supportedMimeTypes = QImageReader::supportedMimeTypes();
}

SDirModel::~SDirModel()
{
}

QPointF SDirModel::positionFor(const KFileItem& item) const
{
	if (!d->iconPositions.contains(dirLister()->url())) {
        d->iconPositions[dirLister()->url()] = QMap<QString, QPointF>();
    }

    auto& map = d->iconPositions[dirLister()->url()];

    if (map.contains(item.name())) {
        return map[item.name()];
    }

	Kirigami::Units units;

	const auto width = units.gridUnit() * 6;
	const auto height = units.gridUnit() * 5;
	const auto overallWidth = units.gridUnit() * (40-7);
	const auto size = QSizeF(width, height);
	QPointF topLeft(0, 0);

    const auto items = dirLister()->items();

	while (std::any_of(items.cbegin(), items.cend(), [=](const KFileItem& item) {
        if (!map.contains(item.name()))
            return false;

		const auto tryingRect = QRectF(topLeft, size);
		const auto comparingRect = QRectF(map[item.name()], size);

		return tryingRect.intersects(comparingRect);
	})) {
		topLeft.rx() += width;
		if (topLeft.x() + width >= overallWidth) {
			topLeft.rx() = 0;
			topLeft.ry() += height;
		}
	}

    map[item.name()] = topLeft;
    return map[item.name()];
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
