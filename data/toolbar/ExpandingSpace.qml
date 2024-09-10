// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon
import QtQml.Models
import QtQuick.Shapes

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
