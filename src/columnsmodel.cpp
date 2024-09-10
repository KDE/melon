// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "columnsmodel.h"
#include "columnsmodel_p.h"
#include "document.h"

SColumnsModel::SColumnsModel(SDocument* parent) : QAbstractListModel(parent), d(new Private)
{

}

SColumnsModel::~SColumnsModel()
{

}

int SColumnsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return d->dirModels.count();
}

QVariant SColumnsModel::data(const QModelIndex& index, int role) const
{
    if (!checkIndex(index))
        return QVariant();

    switch (role) {
    case SColumnsModelRoles::DirModel:
        return QVariant::fromValue(d->dirModels[index.row()].data());
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> SColumnsModel::roleNames() const
{
    return {{SColumnsModelRoles::DirModel, "dirModel"}};
}
