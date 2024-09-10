// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon

QQC2.Control {
	id: control

	required property Melon.Window window
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

			position: QQC2.TabBar.Footer
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