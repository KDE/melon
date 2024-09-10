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
import QtQml

AbstractFileView {
	id: page

	itemCount: -1

	QQC2.ScrollView {
		id: scrollView

		anchors.fill: parent
		clip: true

		QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOn
		QQC2.ScrollBar.vertical.policy: QQC2.ScrollBar.AlwaysOff

		Kirigami.Theme.colorSet: Kirigami.Theme.View
		background: Rectangle {
			Kirigami.Theme.colorSet: Kirigami.Theme.Window
			color: Kirigami.Theme.backgroundColor
		}

		DropIndicator {
			target: scrollView
			visible: page.containsDrag
		}

		contentItem: Flickable {
			id: flickableItem

			contentWidth: columnView.implicitWidth
			contentHeight: columnView.implicitHeight
			contentX: 0

			Binding {
				target: flickableItem
				restoreMode: Binding.RestoreBinding
				property: "contentX"
				value: {
					if (activeColumn === null)
						return previousValue

					const minX = 0
					const centeredX = activeColumn.x - (flickableItem.width/2) + (activeColumn.width/2)
					const maxX = Math.max(0, flickableItem.contentWidth - flickableItem.width)
					previousValue = clamp(minX, centeredX, maxX)
					return clamp(minX, centeredX, maxX)
				}
				property int previousValue: 0
				function clamp(min, num, max) {
					return Math.min(Math.max(num, min), max)
				}
				when: activeColumn !== null

				readonly property Item activeColumn: {
					let item = flickableItem.Window.activeFocusItem
					while (item) {
						if (item.parent === columnView)
							return item
						item = item.parent
					}
					return null
				}
			}

			RowLayout {
				id: columnView
				spacing: 0

				Repeater {
					id: columnRepeater
					model: page.document.columnsModel
					onItemAdded: function(_, item) {
						item.takeFocus()
					}
					delegate: SingleColumn {
						required property Melon.DirModel modelData

						Keys.onRightPressed: {
							openCurrentIndex()

							const item = columnRepeater.itemAt(this.index+1)
							if (item === null) return
							item.takeFocus()
						}
						Keys.onLeftPressed: if (this.index != 0) columnRepeater.itemAt(this.index-1).takeFocus()

						document: page.document
						dirModel: modelData

						implicitWidth: 300
						implicitHeight: scrollView.height - scrollView.QQC2.ScrollBar.horizontal.height
					}
				}
				Kirigami.Separator {
					Layout.fillHeight: true
				}
				FileItemColumn {
					document: page.document
					fileItem: page.document.viewingFileItem
					implicitWidth: 300
					implicitHeight: scrollView.height - scrollView.QQC2.ScrollBar.horizontal.height
				}
				Item { Layout.fillWidth: true }
				// Keys.onSpacePressed: (evt) => currentItem.tap(evt.modifiers)
				// Keys.onEnterPressed: currentItem.doubleTap()
				// Keys.onReturnPressed: currentItem.doubleTap()
				// Keys.onMenuPressed: currentItem.doMenu()
			}
		}
	}
}