import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon
import QtQml.Models

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
			DropIndicator {
				target: scrollView
				visible: page.containsDrag
			}
		}

		contentItem: TableView {
			id: tableView
			activeFocusOnTab: true

			property var columnWidths: [10, 4, 7, 5, 6, 6, 10]
			columnWidthProvider: (column) => columnWidths[column] * Kirigami.Units.gridUnit
			selectionBehavior: TableView.SelectionDisabled
			pointerNavigationEnabled: false

			clip: true
			model: guiModel
			topMargin: headerView.height
			delegate: BaseDelegate {
				id: del
				document: page.document
				dirModel: page.document.dirModel
				index: row

				DropIndicator {
					target: del
					visible: del.containsDrag
				}

				property bool pooled: false
				TableView.onPooled: pooled = true
				TableView.onReused: pooled = false

				background: Rectangle {
					color: {
						if (page.document.rawSelectionModel.selectedIndexes.includes(del.modelIndex))
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
		// delegate: QQC2.AbstractButton {
		// 	id: control
		// 	background: QQC2DesktopStyle.StyleItem {
		// 		elementType: "header"
		// 		sunken: control.down
		// 		hover: control.hovered
		// 		text: model.display || ""
		// 	}
		// }
	}
}