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

QQC2.Control {
	id: del

	required property Melon.Document document
	required property Melon.DirModel dirModel
	required property var decoration
	required property var fileItem
	required property int index
	required property bool isImageReadable
	required property string display
	readonly property var modelIndex: del.dirModel?.index(index, 0)
	readonly property bool containsDrag: dropArea.containsDrag

	DropArea {
		id: dropArea
		enabled: del.fileItem.isDir
		anchors.fill: parent
		onDropped: (event) => del.document.drop(del, event)
	}
	TapHandler {
		acceptedDevices: ~PointerDevice.TouchScreen
		acceptedButtons: Qt.RightButton
		onTapped: doMenu()
	}
	TapHandler {
		acceptedModifiers: Qt.ControlModifier
		acceptedDevices: ~PointerDevice.TouchScreen
		onTapped: (eventPoint) => del.tap(Qt.ControlModifier)
		onDoubleTapped: del.doubleTap()
		onLongPressed: doMenu()
	}
	TapHandler {
		acceptedModifiers: Qt.NoModifier
		acceptedDevices: ~PointerDevice.TouchScreen
		onTapped: (eventPoint) => del.tap(Qt.NoModifier)
		onDoubleTapped: del.doubleTap()
		onLongPressed: doMenu()
	}
	TapHandler {
		acceptedDevices: PointerDevice.TouchScreen
		onTapped: (eventPoint) => del.doubleTap()
		onLongPressed: doMenu()
	}

	function tap(modifiers) {
		const selModel = del.document.selectionModelFor(del.dirModel)
		if (modifiers & Qt.ControlModifier) {
			selModel.select(del.modelIndex, ItemSelectionModel.Select)
		} else {
			selModel.select(del.modelIndex, ItemSelectionModel.ClearAndSelect)
		}
	}
	function doubleTap() {
		if (del.fileItem.isDir) {
			del.document.openUrl(del.fileItem.url)
		} else {
			del.document.openItem(del.fileItem)
		}
	}
	function doMenu() {
		del.document.openRightClickMenuFor(this.fileItem, del.dirModel)
	}
}