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
			}
		}
		delegate: Kirigami.BasicListItem {
			id: del

			required property string iconName
			required property string display
			required property url url

			highlighted: false

			icon: del.iconName
			text: del.display

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

			onClicked: view.document.navigator.currentLocationUrl = del.url
		}
	}
}