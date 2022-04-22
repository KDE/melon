#pragma once

#include <QAbstractProxyModel>
#include <QQmlEngine>
#include <KRearrangeColumnsProxyModel>

class ReverseProxyModel : public KRearrangeColumnsProxyModel
{

	Q_OBJECT
	QML_NAMED_ELEMENT(ReverseProxyModel)

public:
	explicit ReverseProxyModel(QObject *parent = nullptr);
	~ReverseProxyModel();

	bool m_reverse;
	Q_PROPERTY(bool reverse MEMBER m_reverse NOTIFY reverseChanged)
	Q_SIGNAL void reverseChanged();

	void setSourceModel(QAbstractItemModel* sourceModel) override;
};
