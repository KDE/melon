import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj
import QtGraphicalEffects 1.12
import QtQml.Models 2.15
import org.kde.qqc2desktopstyle.private 1.0 as QQC2DesktopStyle

Kirigami.Icon {
	id: icon

	required property Delfenoj.Document document
	required property bool enablePreview
	required property var normalIcon
	required property var fileItem

	source: previewer.pixmapValid ? previewer.pixmap : icon.normalIcon
	visible: Boolean(icon.normalIcon)

	Delfenoj.Previewer {
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
			const diff = new Date() - icon.fileItem.time(Delfenoj.FileItem.CreationTime)
			const days = diff/(60 * 60 * 24 * 1000)
			const power = Math.pow((days / 2000), 3/4)
			return power
		}
	}
}