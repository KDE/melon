import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj
import QtGraphicalEffects 1.12
import QtQml.Models 2.15

QQC2.ToolBar {
	id: control
	required property Delfenoj.Document document

	contentItem: RowLayout {
		QQC2.ToolButton {
			icon.name: "go-previous"
			onClicked: control.document.navigator.goBack()
		}
		QQC2.ToolButton {
			icon.name: "go-next"
			onClicked: control.document.navigator.goForward()
		}
		QQC2.ToolButton {
			icon.name: "go-up"
			onClicked: control.document.navigator.goUp()
		}
		Item { Layout.fillWidth: true }
		QQC2.ToolButton {
			icon.name: "window-new"
			onClicked: delfenojApp.newWindow()
		}
	}
}