#include <KIO/DropJob>
#include <QMimeData>

#include "NGLibQuick.h"
#include "placesdelegate.h"
#include "app.h"

PlacesDelegate::PlacesDelegate(QObject* parent) : NGBeaconControllerDelegate(parent)
{
}

PlacesDelegate::~PlacesDelegate()
{
}

void PlacesDelegate::dragEnterEvent(QDragEnterEvent*)
{
}

void PlacesDelegate::dragMoveEvent(QDragMoveEvent*)
{
}

void PlacesDelegate::dragLeaveEvent(QDragLeaveEvent*)
{
}

void PlacesDelegate::dropEvent(NGBeacon* beacon, QDropEvent* event)
{
	if (beacon == nullptr)
		return;

	auto model = sApp->filePlacesModel;
	auto data = beacon->data();
	auto list = data.toList();
	auto relativeLocation = list[0].toString();
	auto row = list[1].toInt();

	auto idx = model->index(row, 0);

	if (relativeLocation == "on") {
		auto url = model->data(idx, KFilePlacesModel::UrlRole).value<QUrl>();
		KIO::drop(event, url);
		return;
	}

	if (relativeLocation == "before")
		idx = model->index(row - 1, 0);

	model->dropMimeData(event->mimeData(), event->proposedAction(), idx.row(), 0, QModelIndex());
}
