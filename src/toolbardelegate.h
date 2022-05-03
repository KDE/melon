#pragma once

#include "beacons.h"

class ToolBarDelegate : public BeaconControllerDelegate
{
	Q_OBJECT
	QML_NAMED_ELEMENT(ToolBarDelegate)

public:
	explicit ToolBarDelegate(QObject* parent = nullptr);
	~ToolBarDelegate();

	void dragEnterEvent(QDragEnterEvent*) override;
	void dragMoveEvent(QDragMoveEvent*) override;
	void dragLeaveEvent(QDragLeaveEvent*) override;
	void dropEvent(Beacon*, QDropEvent*) override;
};
