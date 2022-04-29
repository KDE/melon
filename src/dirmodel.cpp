#include <QImageReader>

#include "dirmodel.h"

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

QVariant SDirModel::data(const QModelIndex &index, int role) const
{
    auto item = itemForIndex(index);
    if (role != AdditionalRoles::IsImageReadable || item.isNull()) {
        return KDirModel::data(index, role);
    }

    return d->supportedMimeTypes.contains(item.mimetype().toLocal8Bit());
}

QHash<int, QByteArray> SDirModel::roleNames() const
{
    auto names = KDirModel::roleNames();
    names[AdditionalRoles::IsImageReadable] = "isImageReadable";
    return names;
}


