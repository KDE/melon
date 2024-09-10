// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "NGBeaconControllerDelegate.h"

class PlacesDelegate : public NGBeaconControllerDelegate
{
	Q_OBJECT
	QML_NAMED_ELEMENT(PlacesDelegate)

public:
	explicit PlacesDelegate(QObject* parent = nullptr);
	~PlacesDelegate();

	void dragEnterEvent(QDragEnterEvent*) override;
	void dragMoveEvent(QDragMoveEvent*) override;
	void dragLeaveEvent(QDragLeaveEvent*) override;
	void dropEvent(NGBeacon*, QDropEvent*) override;
};
