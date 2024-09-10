// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlParserStatus>
#include <QQuickItem>

class DragThing : public QQuickItem
{
	Q_OBJECT
	QML_NAMED_ELEMENT(DragThing)

	Q_PROPERTY(QJSValue mimeData READ mimeData WRITE setMimeData NOTIFY mimeDataChanged REQUIRED)

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit DragThing(QQuickItem* parent = nullptr);
	~DragThing();

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void hoverEnterEvent(QHoverEvent* event) override;
	void hoverMoveEvent(QHoverEvent* event) override;
	void hoverLeaveEvent(QHoverEvent* event) override;
	Q_INVOKABLE void startDrag();

	QJSValue mimeData() const;
	void setMimeData(QJSValue mimeData);
	Q_SIGNAL void mimeDataChanged();
};
