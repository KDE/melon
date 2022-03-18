import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj
import QtGraphicalEffects 1.12
import QtQml.Models 2.15

QQC2.Page {
	id: page

	required property Delfenoj.Document document

	QQC2.ScrollView {
		anchors.fill: parent

		QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

		Kirigami.Theme.colorSet: Kirigami.Theme.View
		background: Rectangle {
			Kirigami.Theme.colorSet: Kirigami.Theme.Window
			color: Kirigami.Theme.backgroundColor
		}

		ListView {
			model: page.document.dirModel
			activeFocusOnTab: true
			delegate: Kirigami.BasicListItem {
				id: del

				highlighted: page.document.selectionModel.selectedIndexes.includes(del.modelIndex)

				leading: Item {
					width: icon.width
					Kirigami.Icon {
						id: icon
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter
						source: previewer.pixmapValid ? previewer.pixmap : del.decoration

						Delfenoj.Previewer {
							id: previewer
							fileItem: del.fileItem
							size: Qt.size(icon.width, icon.height)
						}

						layer.enabled: true
						layer.effect: Desaturate {
							desaturation: {
								const diff = new Date() - del.fileItem.time(Delfenoj.FileItem.CreationTime)
								const days = diff/(60 * 60 * 24 * 1000)
								const power = Math.pow((days / 2000), 3/4)
								return power
							}
						}
					}
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

				required property string display
				required property var decoration
				required property var fileItem
				required property int index

				readonly property var modelIndex: page.document.dirModel.index(del.index, 0)

				text: del.display
				subtitle: del.fileItem.time(Delfenoj.FileItem.CreationTime).toLocaleString(Locale.ShortFormat)
			}
		}
	}

	header: QQC2.ToolBar {
		contentItem: RowLayout {
			QQC2.ToolButton {
				icon.name: "go-previous"
				onClicked: document.navigator.goBack()
			}
			QQC2.ToolButton {
				icon.name: "go-next"
				onClicked: document.navigator.goForward()
			}
			QQC2.ToolButton {
				icon.name: "go-up"
				onClicked: document.navigator.goUp()
			}
			Item { Layout.fillWidth: true }
			QQC2.ToolButton {
				icon.name: "window-new"
				onClicked: delfenojApp.newWindow()
			}
		}
	}
}