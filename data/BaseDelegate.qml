import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon
import QtGraphicalEffects 1.12
import QtQml.Models 2.15
import org.kde.qqc2desktopstyle.private 1.0 as QQC2DesktopStyle

QQC2.Control {
	id: del

	required property Melon.Document document
	required property Melon.DirModel dirModel
	required property var decoration
	required property var fileItem
	required property int index
	required property bool isImageReadable
	required property string display
	readonly property var modelIndex: del.dirModel.index(index, 0)
	readonly property bool containsDrag: dropArea.containsDrag

	DropArea {
		id: dropArea
		enabled: del.fileItem.isDir
		anchors.fill: parent
		onDropped: (event) => del.document.drop(del, event)
	}
	TapHandler {
		acceptedDevices: PointerDevice.Mouse | PointerDevice.Stylus
		acceptedButtons: Qt.RightButton
		onTapped: doMenu()
	}
	TapHandler {
		acceptedDevices: PointerDevice.Mouse | PointerDevice.Stylus
		onTapped: (eventPoint) => del.tap(eventPoint.event.modifiers)
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