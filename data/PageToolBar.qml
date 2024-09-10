// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon
import Qt5Compat.GraphicalEffects
import QtQml.Models
import Nongurigaeru

ToolBarView {
	id: _toolbarView

	editing: melonApp.editMode
	controller: melonApp.toolbarController

	// edit mode scrim
	Rectangle {
		parent: _toolbarView
		anchors.top: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		height: window.height - _toolbarView.height
		color: Qt.rgba(0, 0, 0, 0.3)
		z: 4
		visible: melonApp.editMode

		BlockMouseArea {
			anchors.fill: parent
		}
		BlockDropArea {
			anchors.fill: parent
		}
	}

	ToolBarPaletteView {
		visible: melonApp.editMode
		controller: melonApp.toolbarController
		outlet: _toolbarView.outlet

		parent: _toolbarView
		anchors.top: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Kirigami.Units.gridUnit
		z: 50001
	}
}