#pragma once

#include "beacons.h"

class PlacesDelegate : public BeaconControllerDelegate
{
	Q_OBJECT
	QML_NAMED_ELEMENT(PlacesDelegate)

public:
	explicit PlacesDelegate(QObject* parent = nullptr);
	~PlacesDelegate();

	void dragEnterEvent(QDragEnterEvent*) override;
	void dragMoveEvent(QDragMoveEvent*) override;
	void dragLeaveEvent(QDragLeaveEvent*) override;
	void dropEvent(Beacon*, QDropEvent*) override;
};
