import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon
import QtQml.Models

AbstractFileView {
	id: page

	itemCount: gridView.count

	QQC2.ScrollView {
		id: scrollView

		anchors.fill: parent

		QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

		Kirigami.Theme.colorSet: Kirigami.Theme.View
		background: Rectangle {
			Kirigami.Theme.colorSet: Kirigami.Theme.Window
			color: Kirigami.Theme.backgroundColor
			DropIndicator {
				target: scrollView
				visible: page.containsDrag
			}
		}

		contentItem: GridView {
			id: gridView

			readonly property int computedWidth: page.width - scrollView.QQC2.ScrollBar.vertical.width
			activeFocusOnTab: true
			highlight: Rectangle {
				visible: gridView.activeFocus
				color: "transparent"
				border.width: 2
				border.color: Kirigami.Theme.highlightColor
				radius: 6
			}
			cellWidth: computedWidth / Math.min(Math.floor(computedWidth / (Kirigami.Units.gridUnit * 8)), 6)
			cellHeight: cellWidth

			Keys.onSpacePressed: (evt) => currentItem.tap(evt.modifiers)
			Keys.onEnterPressed: currentItem.doubleTap()
			Keys.onReturnPressed: currentItem.doubleTap()
			Keys.onMenuPressed: currentItem.doMenu()

			clip: true
			model: page.document.dirModel
			delegate: BaseDelegate {
				id: del
				document: page.document
				dirModel: page.document.dirModel

				width: gridView.cellWidth
				height: gridView.cellHeight

				padding: 2*Kirigami.Units.smallSpacing
				leftInset: Kirigami.Units.smallSpacing
				rightInset: Kirigami.Units.smallSpacing
				bottomInset: Kirigami.Units.smallSpacing
				topInset: Kirigami.Units.smallSpacing

				DropIndicator {
					target: del
					visible: del.containsDrag
				}

				background: Rectangle {
					border.width: 1
					radius: 5

					border.color: {
						if (page.document.rawSelectionModel.selectedIndexes.includes(del.modelIndex))
							return Kirigami.Theme.highlightColor

						return Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.15)
					}
					color: {
						if (page.document.rawSelectionModel.selectedIndexes.includes(del.modelIndex)) {
							const color = Kirigami.Theme.highlightColor
							return Qt.rgba(color.r, color.g, color.b, 0.3)
						}

						return "transparent"
					}
				}
				contentItem: ColumnLayout {
					spacing: 0
					Item {
						Layout.fillWidth: true
						Layout.fillHeight: true

						Loader {
							anchors.fill: parent
							active: del.isImageReadable
							sourceComponent: Image {
								source: del.fileItem.url
								fillMode: Image.PreserveAspectCrop
								asynchronous: true
								mipmap: true
							}
						}
						Loader {
							anchors.centerIn: parent
							active: !del.isImageReadable
							sourceComponent: FileIcon {
								document: page.document
								enablePreview: true
								normalIcon: del.decoration
								fileItem: del.fileItem

								width: Kirigami.Units.iconSizes.enormous
								height: Kirigami.Units.iconSizes.enormous
							}
						}
					}
					Kirigami.Separator {
						Layout.fillWidth: true
					}
					QQC2.Control {
						Layout.fillWidth: true

						leftPadding: Kirigami.Units.smallSpacing*2
						rightPadding: Kirigami.Units.smallSpacing*2
						leftInset: Kirigami.Units.smallSpacing
						rightInset: Kirigami.Units.smallSpacing

						contentItem: RowLayout {
							FileIcon {
								document: page.document
								enablePreview: false
								normalIcon: del.decoration
								fileItem: del.fileItem

								Layout.preferredWidth: Kirigami.Units.iconSizes.small
								Layout.preferredHeight: Kirigami.Units.iconSizes.small
								Layout.alignment: Qt.AlignVCenter
							}
							QQC2.Label {
								text: del.display
								elide: Text.ElideRight
								horizontalAlignment: Text.AlignLeft
								Layout.fillWidth: true
							}
						}
					}
				}
			}
		}
	}
}