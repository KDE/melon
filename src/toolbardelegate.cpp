#include <QMimeData>

#include "app.h"
#include "toolbardelegate.h"
#include "toolbar.h"

ToolBarDelegate::ToolBarDelegate(QObject* parent) : BeaconControllerDelegate(parent)
{

}

ToolBarDelegate::~ToolBarDelegate()
{

}

void ToolBarDelegate::dragEnterEvent(QDragEnterEvent*)
{

}

void ToolBarDelegate::dragMoveEvent(QDragMoveEvent*)
{

}

void ToolBarDelegate::dragLeaveEvent(QDragLeaveEvent*)
{

}

void ToolBarDelegate::dropEvent(Beacon* beacon, QDropEvent* event)
{
	if (beacon == nullptr)
		return;

	auto data = beacon->data();
	auto list = data.toList();
	auto index = list[1].toInt();
	auto before = list[0].toString() == "before";
	auto kind = QString::fromUtf8(event->mimeData()->data("application/x-melon-toolbar-action"));
	auto isDefault = QString::fromUtf8(event->mimeData()->data("application/x-melon-default-set")) == "yes";

	if (isDefault) {
		sApp->toolbar()->resetToolbar();
	} else if (before) {
		sApp->toolbar()->addItemBefore(kind, index);
	} else {
		sApp->toolbar()->addItemAfter(kind, index);
	}
}

