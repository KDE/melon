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

	itemCount: tableView.rows

	Melon.ReverseProxyModel {
		id: guiModel
		sourceModel: page.document.dirModel
		reverse: Qt.application.layoutDirection == Qt.RightToLeft
	}

	QQC2.ScrollView {
		id: scrollView

		anchors.fill: parent

		Kirigami.Theme.colorSet: Kirigami.Theme.View
		background: Rectangle {
			Kirigami.Theme.colorSet: Kirigami.Theme.Window
			color: Kirigami.Theme.backgroundColor
		}

		contentItem: TableView {
			id: tableView

			property var columnWidths: [10, 4, 7, 5, 6, 6, 10]
			columnWidthProvider: (column) => columnWidths[column] * Kirigami.Units.gridUnit

			clip: true
			model: guiModel
			topMargin: headerView.height
			delegate: QQC2.Control {
				id: del

				required property var decoration
				required property var fileItem
				required property int row
				required property int column
				required property string display
				readonly property var modelIndex: page.document.dirModel.index(row, 0)
				property bool pooled: false
				TableView.onPooled: pooled = true
				TableView.onReused: pooled = false

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

				background: Rectangle {
					color: {
						if (page.document.selectionModel.selectedIndexes.includes(del.modelIndex))
							return Kirigami.Theme.highlightColor

						return (row % 2 == 0) ? Kirigami.Theme.alternateBackgroundColor : Kirigami.Theme.backgroundColor
					}
				}
				contentItem: RowLayout {
					FileIcon {
						document: page.document
						enablePreview: !del.pooled
						normalIcon: del.decoration
						fileItem: del.fileItem

						Layout.preferredWidth: Kirigami.Units.iconSizes.small
						Layout.preferredHeight: Kirigami.Units.iconSizes.small
					}
					QQC2.Label {
						text: del.display
						elide: Text.ElideRight
						Layout.fillWidth: true
					}
				}
			}
		}
	}
	QQC2.HorizontalHeaderView {
		id: headerView
		syncView: tableView
		clip: true
		x: Qt.application.layoutDirection == Qt.LeftToRight ?
			0 :
			scrollView.QQC2.ScrollBar.vertical.width
		delegate: QQC2.AbstractButton {
			id: control
			background: QQC2DesktopStyle.StyleItem {
				elementType: "header"
				sunken: control.down
				hover: control.hovered
				text: model.display || ""
			}
		}
	}
}