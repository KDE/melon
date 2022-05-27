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

	itemCount: repeater.count

	QQC2.ScrollView {
		id: scrollView

		anchors.fill: parent

		Kirigami.Theme.colorSet: Kirigami.Theme.View
		background: Rectangle {
			Kirigami.Theme.colorSet: Kirigami.Theme.Window
			color: Kirigami.Theme.backgroundColor
		}

		contentItem: Flickable {
			contentWidth: contentItem.childrenRect.width
			contentHeight: contentItem.childrenRect.height
			clip: true

			Repeater {
				id: repeater

				model: page.document.dirModel
				delegate: BaseDelegate {
					id: del
					document: page.document

					x: del.iconPosition.x
					y: del.iconPosition.y

					width: Kirigami.Units.gridUnit * 6
					height: Kirigami.Units.gridUnit * 5

					background: Rectangle {
						border.width: 1
						radius: 5

						border.color: {
							if (page.document.selectionModel.selectedIndexes.includes(del.modelIndex))
								return Kirigami.Theme.highlightColor

							return "transparent"
						}
						color: {
							if (page.document.selectionModel.selectedIndexes.includes(del.modelIndex)) {
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
}