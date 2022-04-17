import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj

QQC2.ScrollView {
	id: view

	required property Delfenoj.Document document

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
		model: delfenojApp.placesModel
		section.property: "group"
		Delfenoj.BeaconController {
			id: beaconController
			anchors.fill: parent
			delegate: Delfenoj.PlacesDelegate {

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

			width: parent && parent.width > 0 ? parent.width : implicitWidth
			Layout.fillWidth: true

			Delfenoj.Beacon {
				id: aboveBeacon

				controller: beaconController
				beaconID: "above" + del.index
				dataCallback: () => ["above", del.index]

				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
			}

			Delfenoj.Beacon {
				id: belowBeacon

				controller: beaconController
				beaconID: "below" + del.index
				dataCallback: () => ["below", del.index]

				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: parent.right
			}

			Delfenoj.Beacon {
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
				onTapped: view.document.navigator.currentLocationUrl = del.url
			}
			DragHandler {
				target: null
				onActiveChanged: if (active) dragThing.startDrag()
			}
			Delfenoj.DragThing {
				id: dragThing
				Delfenoj.ModelWrapper {
					id: modelWrapper
					model: delfenojApp.placesModel
				}
				mimeData: () => modelWrapper.mimeData([delfenojApp.placesModel.index(del.index, 0)])
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
			}
		}
	}
}