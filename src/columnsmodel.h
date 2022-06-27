#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

class SDocument;

class SColumnsModel : public QAbstractListModel
{

	Q_OBJECT
	QML_NAMED_ELEMENT(ColumnsModel)
	QML_UNCREATABLE("must be accessed from dir model")

	struct Private;
	QScopedPointer<Private> d;

	friend class SDocument;

public:
	explicit SColumnsModel(SDocument* parent);
	~SColumnsModel();

	int rowCount(const QModelIndex& parent = {}) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QHash<int, QByteArray> roleNames() const override;
	Q_SIGNAL void columnsChanged();
};
