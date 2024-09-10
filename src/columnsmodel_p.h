// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "columnsmodel.h"
#include "dirmodel.h"

struct SColumnsModel::Private
{
	QList<QSharedPointer<SDirModel>> dirModels;
};

enum SColumnsModelRoles
{
	DirModel,
};
