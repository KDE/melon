import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon
import QtGraphicalEffects 1.12
import QtQml.Models 2.15
import org.kde.qqc2desktopstyle.private 1.0 as QQC2DesktopStyle

QQC2.ScrollView {
	id: column

	required property Melon.Document document
	required property int index
	required property Melon.DirModel dirModel

	QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
	QQC2.ScrollBar.vertical.policy: QQC2.ScrollBar.AlwaysOn
	Kirigami.Theme.colorSet: Kirigami.Theme.View
	background: Rectangle {
		Kirigami.Theme.colorSet: Kirigami.Theme.Window
		color: Kirigami.Theme.backgroundColor
		DropIndicator {
			target: column
			visible: dropArea.containsDrag
		}
		DropArea {
			id: dropArea
			anchors.fill: parent
			onDropped: (event) => page.document.drop(column, event, column.dirModel.currentURL())
		}
	}

	function incrementCurrentIndex() {
		listView.incrementCurrentIndex()
	}
	function decrementCurrentIndex() {
		listView.decrementCurrentIndex()
	}
	function openCurrentIndex() {
		if (listView.currentItem === null) return
		if (!listView.currentItem.fileItem.isDir) return
		listView.currentItem.document.openColumnUrl(column.index, listView.currentItem.fileItem.url)
	}
	function takeFocus() {
		listView.forceActiveFocus()
	}

	Kirigami.PlaceholderMessage {
		text: melonApp.kaomoji(`¯\\_(ツ)_/¯`)
		explanation: i18n("There are no objects in this directory")
		visible: listView.count === 0 && !page.document.loading
		width: column.width

		parent: column
		anchors.centerIn: parent
	}

	contentItem: ListView {
		id: listView
		activeFocusOnTab: true

		currentIndex: -1
		clip: true
		model: column.dirModel

		highlight: Rectangle {
			color: "transparent"
			border.width: 2
			border.color: listView.activeFocus ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
			opacity: listView.activeFocus ? 1 : 0.25
			radius: 3
			z: 5000
		}
		delegate: BaseDelegate {
			id: del
			document: page.document
			dirModel: column.dirModel
			width: listView.width

			function tap(modifiers) {
				const selModel = del.document.selectionModelFor(del.dirModel)
				if (modifiers & Qt.ControlModifier) {
					selModel.select(del.modelIndex, ItemSelectionModel.Select)
				} else {
					selModel.select(del.modelIndex, ItemSelectionModel.ClearAndSelect)

					if (!del.fileItem.isDir) return
					del.document.openColumnUrl(column.index, del.fileItem.url)
				}
			}

			function doubleTap() {
				if (del.fileItem.isDir) {
					del.document.openColumnUrl(column.index, del.fileItem.url)
				} else {
					del.document.openItem(del.fileItem)
				}
			}

			DropArea {
				id: dropArea
				enabled: del.fileItem.isDir
				anchors.fill: parent
				onDropped: (event) => del.document.drop(del, event)
			}

			DropIndicator {
				target: del
				visible: dropArea.containsDrag
			}

			background: Rectangle {
				color: {
					try {
						if (del.document.rawSelectionModel.selectedIndexes.includes(del.modelIndex))
							return Kirigami.Theme.highlightColor
					} catch (_) { }
					return (del.index % 2 == 0) ? Kirigami.Theme.alternateBackgroundColor : Kirigami.Theme.backgroundColor
				}
			}
			contentItem: RowLayout {
				FileIcon {
					document: page.document
					enablePreview: true
					normalIcon: del.decoration
					fileItem: del.fileItem

					Layout.preferredWidth: Kirigami.Units.iconSizes.small
					Layout.preferredHeight: Kirigami.Units.iconSizes.small
					Layout.alignment: Qt.AlignVCenter
				}
				QQC2.Label {
					text: del.display
					elide: Text.ElideRight
					Layout.fillWidth: true
				}
				Kirigami.Icon {
					source: "arrow-right"

					visible: del.fileItem.isDir

					Layout.preferredWidth: Kirigami.Units.iconSizes.small
					Layout.preferredHeight: Kirigami.Units.iconSizes.small
					Layout.alignment: Qt.AlignVCenter
				}
			}
		}
	}
}