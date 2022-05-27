#include <QImageReader>
#include <Kirigami/Units>
#include <KDirLister>

#include <sys/xattr.h>
#include <optional>

#include "dirmodel.h"

std::optional<QPointF> getXAttr(const KFileItem& item)
{
    QByteArray data(32, '\0');
    auto ret = lgetxattr(qPrintable(item.localPath()), "user.org.kde.melon.pos", data.data(), data.size());
    if (ret < 0) {
        return {};
    }
    QDataStream stream(data);
    QPointF pos;
    stream >> pos;
    return {pos};
}

auto setXAttr(const KFileItem& item, const QPointF& pos)
{
    QByteArray data(32, '\0');
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << pos;
    lsetxattr(qPrintable(item.localPath()), "user.org.kde.melon.pos", data.data(), data.size(), 0);
}

struct SDirModel::Private
{
	// TODO(microoptimisation): don't have a copy of this in every SDirModel
	QList<QByteArray> supportedMimeTypes;
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
    if (auto pos = getXAttr(item)) {
        return *pos;
    }

	Kirigami::Units units;

	const auto width = units.gridUnit() * 6;
	const auto height = units.gridUnit() * 5;
	const auto overallWidth = units.gridUnit() * (40-7);
	const auto size = QSizeF(width, height);
	QPointF topLeft(0, 0);

    const auto items = dirLister()->items();

	while (std::any_of(items.cbegin(), items.cend(), [=](const KFileItem& item) {
		auto comparingPos = getXAttr(item);
		if (!comparingPos.has_value())
			return false;

		const auto tryingRect = QRectF(topLeft, size);
		const auto comparingRect = QRectF(comparingPos.value(), size);

		return tryingRect.intersects(comparingRect);
	})) {
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
