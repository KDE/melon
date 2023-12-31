import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon
import QtQml.Models

QQC2.ToolButton {
	id: control
	icon.name: "go-next"
	property Melon.Document outlet
	onClicked: control.outlet.navigator.goForward()
}
