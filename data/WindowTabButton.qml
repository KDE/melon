import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon

QQC2.TabButton {
	id: del

	required property int index
	required property Melon.Document modelData

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
