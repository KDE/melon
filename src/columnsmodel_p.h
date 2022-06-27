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
