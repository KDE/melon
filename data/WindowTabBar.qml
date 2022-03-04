import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj

QQC2.Control {
	id: control

	required property Delfenoj.Window window
	readonly property int currentIndex: tabBar.currentIndex

	topPadding: 0
	leftPadding: 0
	rightPadding: 0
	bottomPadding: 0

	DropArea {
		id: dropArea

		anchors.fill: parent
		onDropped: (event) => {
			if (event.source instanceof WindowTabButton) {
				event.source.modelData.moveTo(control.window)
			} else if (event.hasUrls) {
				// TODO: handle urls
			}
		}
	}

	background: Rectangle {
		color: Kirigami.Theme.backgroundColor
	}
	contentItem: RowLayout {
		QQC2.TabBar {
			id: tabBar

			Layout.fillWidth: true

			Repeater {
				model: control.window.documents
				delegate: WindowTabButton {

				}
			}
		}
		QQC2.ToolButton {
			icon.name: "list-add"
			onClicked: control.window.newDocument()
		}
	}
}