#include "NGApplication.h"
#include "NGRestorer.h"

#include <QDrag>
#include <QMimeData>
#include <QQuickItemGrabResult>
#include <QQuickWindow>
#include <QStyleHints>
#include <QQmlEngine>

#include "dragthing.h"

struct DragThing::Private
{
	QQuickItem* item = nullptr;
	QJSValue mimeData;
	QPoint startPos;
	QPixmap pixmap;
};

DragThing::DragThing(QQuickItem* parent) : QQuickItem(parent), d(new Private)
{
	setAcceptedMouseButtons(Qt::LeftButton);
	setAcceptHoverEvents(true);
}

DragThing::~DragThing()
{
}

void DragThing::hoverEnterEvent(QHoverEvent* event)
{
	d->startPos = event->pos();
}

void DragThing::hoverMoveEvent(QHoverEvent* event)
{
	d->startPos = event->pos();
}

void DragThing::hoverLeaveEvent(QHoverEvent* event)
{
	Q_UNUSED(event);

	d->startPos = QPoint();
}

void DragThing::startDrag()
{
	QDrag* drag = new QDrag(window());
	drag->setPixmap(d->pixmap);
	drag->setHotSpot(d->startPos);
	auto mimeData = d->mimeData.call().toVariant().value<QMimeData*>();
	qmlEngine(this)->setObjectOwnership(mimeData, QQmlEngine::CppOwnership);
	drag->setMimeData(mimeData);
	drag->exec(Qt::MoveAction | Qt::CopyAction | Qt::LinkAction, Qt::CopyAction);
}

void DragThing::mousePressEvent(QMouseEvent* event)
{
	auto img = parentItem()->grabToImage();
	connect(img.get(), &QQuickItemGrabResult::ready, this, [img, this] {
		d->pixmap = QPixmap::fromImage(img->image());
	});

	event->ignore();
}

// void DragThing::mouseMoveEvent(QMouseEvent* event)
// {
// 	if (d->startPos.isNull())
// 	{
// 		event->ignore();
// 	}

// 	auto newPos = event->pos();
// 	auto sub = (d->startPos - newPos).manhattanLength();
// 	auto isLonger = sub >= qApp->styleHints()->startDragDistance();

// 	if (isLonger && !d->mimeData.isNull()) {

// 	}
// }

// void DragThing::mouseReleaseEvent(QMouseEvent* event)
// {
//     Q_UNUSED(event)

// 	d->startPos = QPoint();
// }

QJSValue DragThing::mimeData() const
{
	return d->mimeData;
}

void DragThing::setMimeData(QJSValue mimeData)
{
	d->mimeData = mimeData;
	Q_EMIT mimeDataChanged();
}

static NGMetaTypeRegisterer<QMimeData*> registerer;
