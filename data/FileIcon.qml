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

Kirigami.Icon {
	id: icon

	required property Melon.Document document
	required property bool enablePreview
	required property var normalIcon
	required property var fileItem

	source: previewer.pixmapValid ? previewer.pixmap : icon.normalIcon
	visible: Boolean(icon.normalIcon)

	Melon.Previewer {
		id: previewer
		item: icon
		fileItem: icon.fileItem
		enabled: icon.enablePreview && Boolean(icon.normalIcon)
		size: Qt.size(icon.width, icon.height)
	}

	DragHandler {
		target: null
		onActiveChanged: if (active) icon.document.startDrag()
	}

	layer.enabled: true
	layer.effect: Desaturate {
		desaturation: {
			const diff = new Date() - icon.fileItem.time(Melon.FileItem.CreationTime)
			const days = diff/(60 * 60 * 24 * 1000)
			const power = Math.pow((days / 2000), 3/4)
			return power
		}
	}
}