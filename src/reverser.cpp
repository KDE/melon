#include "reverser.h"

ReverseProxyModel::ReverseProxyModel(QObject* parent) : KRearrangeColumnsProxyModel(parent)
{
	connect(this, &ReverseProxyModel::reverseChanged, this, [this] {
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

	auto columns = QVector<int>(sourceModel()->columnCount());
	std::iota(columns.begin(), columns.end(), 0);
	if (m_reverse) {
		std::reverse(columns.begin(), columns.end());
	}
	setSourceColumns(columns);
}
