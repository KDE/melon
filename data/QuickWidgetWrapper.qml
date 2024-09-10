// SPDX-FileCopyrightText: 2024 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2

Item {
	id: _wrapper
	property Item child
	onChildChanged: {
		child.anchors.fill = _wrapper
	}
}