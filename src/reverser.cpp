// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "reverser.h"

ReverseProxyModel::ReverseProxyModel(QObject* parent) : KRearrangeColumnsProxyModel(parent)
{
	connect(this, &ReverseProxyModel::reverseChanged, this, [this] {
		if (sourceModel() == nullptr) { return; }
		auto columns = QVector<int>(sourceModel()->columnCount());
		std::iota(columns.begin(), columns.end(), 0);
		if (m_reverse) {
			std::reverse(columns.begin(), columns.end());
		}
		setSourceColumns(columns);
	});
}

ReverseProxyModel::~ReverseProxyModel()
{
}

void ReverseProxyModel::setSourceModel(QAbstractItemModel* model)
{
	KRearrangeColumnsProxyModel::setSourceModel(model);
	if (sourceModel() == nullptr) { return; }

	auto columns = QVector<int>(sourceModel()->columnCount());
	std::iota(columns.begin(), columns.end(), 0);
	if (m_reverse) {
		std::reverse(columns.begin(), columns.end());
	}
	setSourceColumns(columns);
}
