// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractItemModel>
#include <QQmlEngine>
#include <QMimeData>

class ModelWrapper : public QObject
{

	Q_OBJECT
	QML_NAMED_ELEMENT(ModelWrapper)

public:
	Q_PROPERTY(QAbstractItemModel* model MEMBER model)
	QAbstractItemModel* model;

	ModelWrapper(QObject* parent = nullptr) : QObject(parent) { }
	~ModelWrapper();

	Q_INVOKABLE QMimeData* mimeData(const QModelIndexList& indexes) const
	{
		return model->mimeData(indexes);
	}
};
