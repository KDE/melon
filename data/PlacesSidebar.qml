import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon

QQC2.ScrollView {
	id: view

	required property Melon.Document document

	QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

	Layout.fillHeight: true
	Layout.preferredWidth: Kirigami.Units.gridUnit*7

	Kirigami.Theme.colorSet: Kirigami.Theme.View
	background: Rectangle {
		Kirigami.Theme.colorSet: Kirigami.Theme.Window
		color: Kirigami.Theme.backgroundColor
	}

	readonly property int betweenTargetSize: 12

	ListView {
		id: listView
		model: melonApp.placesModel
		activeFocusOnTab: true
		section.property: "group"
		highlight: Rectangle {
			visible: listView.activeFocus
			color: "transparent"
			border.width: 2
			border.color: Kirigami.Theme.highlightColor
			radius: 3
			z: 100
		}
		Keys.onSpacePressed: currentItem.tap()

		Melon.BeaconController {
			id: beaconController
			anchors.fill: parent
			delegate: Melon.PlacesDelegate {

			}
		}
		section.delegate: Kirigami.ListSectionHeader {
			id: del

			label: section
			contentItem: QQC2.Label {
				text: del.label
				elide: Text.ElideRight
			}
		}
		delegate: QQC2.Control {
			id: del

			required property string iconName
			required property string display
			required property var index
			required property url url
			required property bool isSetupNeeded
			required property bool isTeardownAllowed
			required property bool isEjectAllowed

			width: parent && parent.width > 0 ? parent.width : implicitWidth
			Layout.fillWidth: true

			Melon.Beacon {
				id: aboveBeacon

				controller: beaconController
				beaconID: "above" + del.index
				dataCallback: () => ["above", del.index]

				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
			}

			Melon.Beacon {
				id: belowBeacon

				controller: beaconController
				beaconID: "below" + del.index
				dataCallback: () => ["below", del.index]

				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: parent.right
			}

			Melon.Beacon {
				id: onBeacon

				controller: beaconController
				beaconID: "on" + del.index
				dataCallback: () => ["on", del.index]

				anchors.top: parent.top
				anchors.topMargin: view.betweenTargetSize
				anchors.bottom: parent.bottom
				anchors.bottomMargin: view.betweenTargetSize
				anchors.left: parent.left
				anchors.right: parent.right
			}

			Rectangle {
				anchors.top: parent.top
				anchors.topMargin: -height/2
				height: view.betweenTargetSize/2
				width: parent.width

				color: Kirigami.Theme.highlightColor
				visible: beaconController.activeBeaconID == aboveBeacon.beaconID

				z: 500
			}
			Rectangle {
				anchors.bottom: parent.bottom
				anchors.bottomMargin: -height/2
				height: view.betweenTargetSize/2
				width: parent.width

				color: Kirigami.Theme.highlightColor
				visible: beaconController.activeBeaconID == belowBeacon.beaconID

				z: 500
			}

			HoverHandler {
				id: hoverHandler
			}
			TapHandler {
				id: tapHandler
				onTapped: tap()
				onLongPressed: doMenu()
			}
			TapHandler {
				acceptedButtons: Qt.RightButton
				onTapped: doMenu()
			}
			function tap() {
				view.document.openUrl(del.url)
			}
			function doMenu() {
				melonApp.openRightClickMenuForPlace(melonApp.placesModel.index(del.index, 0))
			}
			DragHandler {
				target: null
				onActiveChanged: if (active) dragThing.startDrag()
			}
			Melon.DragThing {
				id: dragThing
				Melon.ModelWrapper {
					id: modelWrapper
					model: melonApp.placesModel
				}
				mimeData: () => modelWrapper.mimeData([melonApp.placesModel.index(del.index, 0)])
				parent: del
				anchors.fill: parent
			}

			background: ListItemBackground {
				hovered: hoverHandler.hovered || beaconController.activeBeaconID == onBeacon.beaconID
				pressed: tapHandler.pressed
				item: del
			}
			contentItem: RowLayout {
				Kirigami.Icon {
					source: del.iconName

					Layout.preferredWidth: Kirigami.Units.iconSizes.small
					Layout.preferredHeight: Kirigami.Units.iconSizes.small
					Layout.alignment: Qt.AlignVCenter
				}
				QQC2.Label {
					font: Kirigami.Theme.smallFont
					text: del.display
					elide: Text.ElideRight

					Layout.fillWidth: true
				}
				SmallIconButton {
					icon.name: "media-mount"
					visible: del.isSetupNeeded
					onClicked: melonApp.placesModel.requestSetup(melonApp.placesModel.index(del.index, 0))
				}
				SmallIconButton {
					icon.name: "media-eject"
					visible: del.isTeardownAllowed && !del.isEjectAllowed
					onClicked: melonApp.placesModel.requestTeardown(melonApp.placesModel.index(del.index, 0))
				}
				SmallIconButton {
					icon.name: "media-eject"
					visible: del.isEjectAllowed
					onClicked: melonApp.placesModel.requestEject(melonApp.placesModel.index(del.index, 0))
				}
			}
		}
	}
}