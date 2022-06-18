import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon
import QtGraphicalEffects 1.12
import QtQml.Models 2.15
import org.kde.qqc2desktopstyle.private 1.0 as QQC2DesktopStyle
import QtQuick.Shapes 1.15

Rectangle {
	id: rectangle

	color: "transparent"
	border.color: melonApp.editMode ? Kirigami.Theme.textColor : "transparent"
	border.width: 1

	Shape {
		visible: melonApp.editMode
		anchors.fill: parent

		// line across
		ShapePath {
			strokeColor: Kirigami.Theme.textColor

			startX: 1
			startY: Math.floor(rectangle.height/2)

			PathLine { x: rectangle.width; y: Math.floor(rectangle.height/2) }
		}

		// right arrow cap top
		ShapePath {
			strokeColor: Kirigami.Theme.textColor

			startX: rectangle.width
			startY: Math.floor(rectangle.height/2)

			PathLine { x: rectangle.width - 5; y: Math.floor(rectangle.height/2) - 5 }
		}

		// right arrow cap bottom
		ShapePath {
			strokeColor: Kirigami.Theme.textColor

			startX: rectangle.width
			startY: Math.floor(rectangle.height/2)

			PathLine { x: rectangle.width - 5; y: Math.floor(rectangle.height/2) + 5 }
		}

		// left arrow cap top
		ShapePath {
			strokeColor: Kirigami.Theme.textColor

			startX: 0
			startY: Math.floor(rectangle.height/2)

			PathLine { x: 5; y: Math.floor(rectangle.height/2) - 5 }
		}

		// left arrow cap bottom
		ShapePath {
			strokeColor: Kirigami.Theme.textColor

			startX: 0
			startY: Math.floor(rectangle.height/2)

			PathLine { x: 5; y: Math.floor(rectangle.height/2) + 5 }
		}
	}

	property Melon.Document outlet
	Layout.fillWidth: true
	Layout.fillHeight: true
	implicitWidth: 20
	implicitHeight: 20
}
