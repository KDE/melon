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

    void paint(QPainter* painter) override;
	void hoverEnterEvent(QHoverEvent* event) override;
	void hoverLeaveEvent(QHoverEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

};
