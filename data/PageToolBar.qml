import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon
import QtGraphicalEffects 1.12
import QtQml.Models 2.15
import Nongurigaeru 1.0

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