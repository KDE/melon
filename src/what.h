#pragma once

#include <QQuickPaintedItem>

class SDocument;

class What : public QQuickPaintedItem
{
	Q_OBJECT
	QML_NAMED_ELEMENT(What)

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit What(QQuickItem* parent = nullptr);
	~What();

    void paint (QPainter* painter) override;

};
