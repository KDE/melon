// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon
import Qt5Compat.GraphicalEffects
import QtQml.Models

QQC2.ToolBar {
	id: bar

	required property Melon.Document document

	implicitHeight: topPadding + contentItem.implicitHeight + bottomPadding

	contentItem: RowLayout {
		spacing: Kirigami.Units.smallSpacing
		Repeater {
			model: bar.document.pathSegmentStrings
			delegate: RowLayout {
				id: del

				layoutDirection: Qt.application.layoutDirection
				required property string modelData
				required property int index

				QQC2.Label {
					text: del.modelData
					font: Kirigami.Theme.smallFont
					verticalAlignment: Text.AlignVCenter

					Layout.fillHeight: true

					TapHandler {
						onDoubleTapped: bar.document.openUrl(bar.document.pathSegmentURLs[del.index])
					}
				}

				Kirigami.Icon {
					visible: del.index+1 != bar.document.pathSegmentStrings.length
					source: LayoutMirroring.enabled ? "arrow-left" : "arrow-right"

					Layout.alignment: Qt.AlignVCenter
					Layout.preferredWidth: Kirigami.Units.iconSizes.small
					Layout.preferredHeight: Kirigami.Units.iconSizes.small
				}
			}
		}
		Item { Layout.fillWidth: true }
	}
}