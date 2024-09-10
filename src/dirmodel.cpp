// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QImageReader>
#include <KDirLister>

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

QUrl SDirModel::currentURL() const
{
    return dirLister()->url();
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

QSharedPointer<SDirModel> SDirModel::duplicate()
{
    auto model = QSharedPointer<SDirModel>::create(nullptr);
    model->openUrl(dirLister()->url());
    return model;
}
