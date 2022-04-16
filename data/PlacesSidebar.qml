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

	ListView {
		model: delfenojApp.placesModel
		section.property: "group"
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

			background: ListItemBackground {
				hovered: hoverHandler.hovered
				pressed: tapHandler.pressed
				item: del
			}

			width: parent && parent.width > 0 ? parent.width : implicitWidth
			Layout.fillWidth: true

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