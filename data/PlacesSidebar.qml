import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj

QQC2.ScrollView {
	id: view

	required property Delfenoj.Document document

	Layout.fillHeight: true
	Layout.preferredWidth: Kirigami.Units.gridUnit*10

	ListView {
		model: delfenojApp.placesModel
		section.property: "group"
		section.delegate: Kirigami.ListSectionHeader {
			label: section
		}
		delegate: Kirigami.BasicListItem {
			id: del

			required property string iconName
			required property string display
			required property url url

			highlighted: false

			icon: del.iconName
			text: del.display

			onClicked: view.document.navigator.currentLocationUrl = del.url
		}
	}
}