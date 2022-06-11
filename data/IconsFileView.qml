import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon
import QtGraphicalEffects 1.12
import QtQml.Models 2.15
import org.kde.qqc2desktopstyle.private 1.0 as QQC2DesktopStyle

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
			activeFocusOnTab: true
			highlight: Rectangle {
				visible: gridView.activeFocus
				color: "transparent"
				border.width: 2
				border.color: Kirigami.Theme.highlightColor
				radius: 6
			}

			Keys.onSpacePressed: (evt) => currentItem.tap(evt.modifiers)
			Keys.onEnterPressed: currentItem.doubleTap()
			Keys.onReturnPressed: currentItem.doubleTap()
			Keys.onMenuPressed: currentItem.doMenu()

			cellWidth: Kirigami.Units.gridUnit * 6
			cellHeight: Kirigami.Units.gridUnit * 5

			clip: true
			model: page.document.dirModel
			delegate: BaseDelegate {
				id: del
				document: page.document
				dirModel: page.document.dirModel

				width: Kirigami.Units.gridUnit * 6
				height: Kirigami.Units.gridUnit * 5

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

						return "transparent"
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
					FileIcon {
						document: page.document
						enablePreview: true
						normalIcon: del.decoration
						fileItem: del.fileItem

						Layout.preferredWidth: Kirigami.Units.iconSizes.large
						Layout.preferredHeight: Kirigami.Units.iconSizes.large
						Layout.alignment: Qt.AlignHCenter
					}
					QQC2.Label {
						text: del.display
						elide: Text.ElideRight
						wrapMode: Text.Wrap
						maximumLineCount: 2
						horizontalAlignment: Text.AlignHCenter
						Layout.fillWidth: true
					}
					Item { Layout.fillHeight: true }
				}
			}
		}
	}
}