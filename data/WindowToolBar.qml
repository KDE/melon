// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon

QQC2.ToolBar {
	id: control

    position: QQC2.ToolBar.Footer

	required property Melon.Window window
    required property int currentIndex

    readonly property Melon.Document document: control.window.documents[control.currentIndex]
    implicitHeight: topPadding + contentItem.implicitHeight + bottomPadding

	contentItem: RowLayout {
        Kirigami.Icon {
            source: "emblem-readonly"
            visible: !control.document.writable
            Layout.preferredWidth: Kirigami.Units.iconSizes.small
            Layout.preferredHeight: Kirigami.Units.iconSizes.small
            Layout.alignment: Qt.AlignVCenter
        }
        Kirigami.Icon {
            source: "emblem-mounted"
            visible: !control.document.local
            Layout.preferredWidth: Kirigami.Units.iconSizes.small
            Layout.preferredHeight: Kirigami.Units.iconSizes.small
            Layout.alignment: Qt.AlignVCenter
        }
        QQC2.Label {
            font: Kirigami.Theme.smallFont
            text: i18n("%1 File(s), %2 Folder(s), %3", control.document.numberOfFiles, control.document.numberOfFolders, control.document.dirSize)
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }
	}
}