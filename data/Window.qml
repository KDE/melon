import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj

QQC2.ApplicationWindow {
	id: window

	visible: true
	title: "Delfenoj"

	required property Delfenoj.Document document

	header: QQC2.ToolBar {
		contentItem: RowLayout {
			QQC2.ToolButton {
				icon.name: "go-previous"
				onClicked: document.navigator.goBack()
			}
			QQC2.ToolButton {
				icon.name: "go-next"
				onClicked: document.navigator.goForward()
			}
			QQC2.ToolButton {
				icon.name: "go-up"
				onClicked: document.navigator.goUp()
			}
			Item { Layout.fillWidth: true }
		}
	}

	QQC2.ScrollView {
		anchors.fill: parent

		Kirigami.Theme.colorSet: Kirigami.Theme.View
		background: Rectangle {
			Kirigami.Theme.colorSet: Kirigami.Theme.Window
			color: Kirigami.Theme.backgroundColor
		}

		ListView {
			model: window.document.dirModel
			activeFocusOnTab: true
			delegate: Kirigami.BasicListItem {
				id: del

				leading: Item {
					width: icon.width
					Kirigami.Icon {
						id: icon
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter
						source: del.decoration
					}
				}

				required property string display
				required property var decoration
				required property var fileItem

				onClicked: window.document.navigator.currentLocationUrl = fileItem.url

				text: del.display
				subtitle: del.fileItem.mimetype
			}
		}
	}
}
