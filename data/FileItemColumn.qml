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

QQC2.ScrollView {
	id: column
	visible: !fileItem.isNull && !fileItem.isDir

	required property var fileItem
	required property Melon.Document document

	QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
	QQC2.ScrollBar.vertical.policy: QQC2.ScrollBar.AlwaysOn
	Kirigami.Theme.colorSet: Kirigami.Theme.View
	background: Rectangle {
		Kirigami.Theme.colorSet: Kirigami.Theme.View
		color: Kirigami.Theme.backgroundColor
	}

	contentItem: Flickable {
		id: flickable

		readonly property real availableWidth: column.width - column.QQC2.ScrollBar.vertical.width

		contentWidth: availableWidth
		contentHeight: columnLayout.implicitHeight

		ColumnLayout {
			id: columnLayout
			width: flickable.availableWidth

			FileIcon {
				document: column.document
				enablePreview: true
				normalIcon: column.fileItem.iconName
				fileItem: column.fileItem

				implicitWidth: Kirigami.Units.iconSizes.enormous
				implicitHeight: Kirigami.Units.iconSizes.enormous
				Layout.alignment: Qt.AlignHCenter
			}

			GridLayout {
				columns: 2
				Layout.alignment: Qt.AlignHCenter
				Layout.maximumWidth: flickable.availableWidth - Layout.leftMargin - Layout.rightMargin
				Layout.leftMargin: Kirigami.Units.smallSpacing
				Layout.rightMargin: Kirigami.Units.smallSpacing

				component ItemLabel : SelectableLabel {
					horizontalAlignment: TextEdit.AlignRight
					verticalAlignment: TextEdit.AlignVCenter
					font: Kirigami.Theme.smallFont
					opacity: 0.5
					selectByMouse: false
					Layout.alignment: Qt.AlignRight
					Layout.fillHeight: true
				}
				component ItemData : SelectableLabel {
					horizontalAlignment: TextEdit.AlignLeft
					verticalAlignment: TextEdit.AlignVCenter
					font: Kirigami.Theme.smallFont
					Layout.maximumWidth: flickable.width * (3/5)
					Layout.alignment: Qt.AlignLeft
					Layout.fillHeight: true
				}

				ItemLabel {
					text: i18n("Name")
				}
				ItemData {
					Layout.alignment: Qt.AlignLeft
					text: column.fileItem.name
				}
				ItemLabel {
					text: i18n("Type")
				}
				ItemData {
					text: column.fileItem.mimeComment
				}
				ItemLabel {
					text: i18n("Size")
				}
				ItemData {
					text: column.fileItem.sizeString
				}
				ItemLabel {
					text: i18n("Created")
				}
				ItemData {
					text: column.fileItem.time(Melon.FileItem.CreationTime).toLocaleString(Locale.ShortFormat)
				}
				ItemLabel {
					text: i18n("Changed")
				}
				ItemData {
					text: column.fileItem.time(Melon.FileItem.ModificationTime).toLocaleString(Locale.ShortFormat)
				}
				ItemLabel {
					text: i18n("Last Opened")
				}
				ItemData {
					text: column.fileItem.time(Melon.FileItem.AccessTime).toLocaleString(Locale.ShortFormat)
				}
			}
			QQC2.Button {
				text: i18n("More Information")
				onClicked: column.document.getInfoOnFile(column.fileItem)
				Layout.alignment: Qt.AlignHCenter
			}
			Item { Layout.preferredWidth: flickable.availableWidth }
		}
	}
}