// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon
import QtQml.Models

Kirigami.ShadowedRectangle {
	color: Kirigami.Theme.backgroundColor
	border.width: 1
	border.color: Kirigami.ColorUtils.tintWithAlpha(color, Kirigami.Theme.textColor, 0.2)
	radius: 3
	shadow {
		size: Kirigami.Units.gridUnit
		color: Qt.rgba(0, 0, 0, 0.2)
		yOffset: 2
	}
}