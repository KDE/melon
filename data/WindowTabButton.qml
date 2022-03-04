import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj

QQC2.TabButton {
	id: del

	required property int index
	required property Delfenoj.Document modelData

	Drag.dragType: Drag.Automatic
	Drag.active: dragHandler.active
	Drag.mimeData: {
		"text/uri-list": [del.modelData.navigator.currentLocationUrl]
	}
	Drag.onDragStarted: {
		del.grabToImage(function(result) {
			del.Drag.imageSource = result.url
		})
	}

	DragHandler {
		id: dragHandler
		target: null
	}

	implicitWidth: implicitContentWidth + leftPadding + rightPadding
	leftPadding: Kirigami.Units.largeSpacing
	contentItem: RowLayout {
		QQC2.Label {
			text: del.modelData.title
			horizontalAlignment: Qt.AlignHCenter

			Layout.fillWidth: true
			Layout.minimumWidth: Kirigami.Units.gridUnit * 3
		}
		QQC2.ToolButton {
			icon.name: "tab-close"
			onClicked: del.modelData.window.closeDocument(del.index)
		}
	}
}
