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
		}

		contentItem: GridView {
			id: gridView

			cellWidth: Kirigami.Units.gridUnit * 6
			cellHeight: Kirigami.Units.gridUnit * 5

			clip: true
			model: page.document.dirModel
			delegate: QQC2.Control {
				id: del

				required property var decoration
				required property var fileItem
				required property int index
				required property string display
				readonly property var modelIndex: page.document.dirModel.index(index, 0)

				width: Kirigami.Units.gridUnit * 6
				height: Kirigami.Units.gridUnit * 5

				DropArea {
					anchors.fill: parent
					onDropped: (event) => page.document.drop(del, event)
				}
				TapHandler {
					acceptedButtons: Qt.RightButton
					onTapped: doMenu()
				}
				TapHandler {
					onTapped: (eventPoint) => {
						const selModel = page.document.selectionModel
						if (eventPoint.event.modifiers & Qt.ControlModifier) {
							selModel.select(del.modelIndex, ItemSelectionModel.Select)
						} else {
							selModel.select(del.modelIndex, ItemSelectionModel.ClearAndSelect)
						}
					}
					onDoubleTapped: if (del.fileItem.isDir) {
						page.document.navigator.currentLocationUrl = del.fileItem.url
					} else {
						page.document.openItem(del.fileItem)
					}
					onLongPressed: doMenu()
				}

				function doMenu() {
					page.document.openRightClickMenuFor(this.fileItem)
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