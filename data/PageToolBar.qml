import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon
import QtGraphicalEffects 1.12
import QtQml.Models 2.15

QQC2.ToolBar {
	id: control
	required property Melon.Document document

	Melon.BeaconController {
		id: beaconController
		parent: control
		anchors.fill: parent
		delegate: Melon.ToolBarDelegate { }
	}

	// edit mode scrim
	Rectangle {
		parent: control
		anchors.top: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		height: window.height - control.height
		color: Qt.rgba(0, 0, 0, 0.3)
		z: 4
		visible: melonApp.editMode

		BlockMouseArea {
			anchors.fill: parent
		}
		BlockDropArea {
			anchors.fill: parent
		}
	}

	// the palette of tools itself
	QQC2.Control {
		parent: control
		anchors.top: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Kirigami.Units.gridUnit
		z: 5
		visible: melonApp.editMode
		padding: Kirigami.Units.gridUnit
		leftPadding: padding
		rightPadding: padding
		topPadding: padding
		bottomPadding: padding

		background: ToolBarEditorBackground { }
		contentItem: ColumnLayout {
			spacing: Kirigami.Units.largeSpacing
			QQC2.Label {
				text: i18n("Drag your favourite objects into the toolbar...")
				font.bold: true
				Layout.fillWidth: true
			}
			Flow {
				spacing: Kirigami.Units.gridUnit
				Layout.margins: Kirigami.Units.largeSpacing
				Layout.fillWidth: true

				Repeater {
					model: melonApp.toolbar.possibleItems
					delegate: ColumnLayout {
						id: del
						required property var modelData

						DragHandler {
							target: null
							onActiveChanged: {
								if (active) {
									loader.item.grabToImage((result) => {
										del.Drag.imageSource = result.url
										del.Drag.active = active
									})
								} else {
									del.Drag.active = active
								}
							}
						}
						Drag.dragType: Drag.Automatic
						Drag.mimeData: {
							"application/x-melon-toolbar-action": del.modelData.itemKind
						}

						Loader {
							id: loader

							sourceComponent: del.modelData.itemComponent
							onItemChanged: {
								maus.parent = loader.item
								maus.anchors.fill = loader.item
							}

							Binding {
								target: loader.item
								value: control.document
								property: "document"
							}
							BlockMouseArea {
								id: maus
							}

							Layout.alignment: Qt.AlignHCenter
						}
						QQC2.Label {
							text: del.modelData.guiName
							horizontalAlignment: Qt.AlignHCenter

							Layout.fillWidth: true
						}
					}
				}
			}
			QQC2.Label {
				text: i18n("... or drag the default set into the toolbar")
				font.bold: true
				Layout.fillWidth: true
			}
			QQC2.Control {
				id: defaultSet

				DragHandler {
					target: null
					onActiveChanged: {
						if (active) {
							defaultSet.grabToImage((result) => {
								defaultSet.Drag.imageSource = result.url
								defaultSet.Drag.active = active
							})
						} else {
							defaultSet.Drag.active = active
						}
					}
				}
				Drag.dragType: Drag.Automatic
				Drag.mimeData: {
					"application/x-melon-default-set": "yes"
				}

				background: Rectangle {
					color: Kirigami.Theme.backgroundColor
					border.color: Kirigami.ColorUtils.tintWithAlpha(color, Kirigami.Theme.textColor, 0.2)
					border.width: 1
				}
				contentItem: RowLayout {
					Repeater {
						model: melonApp.toolbar.defaultItems
						delegate: Loader {
							id: loader

							required property var modelData
							required property int index
							sourceComponent: modelData.itemComponent

							Binding {
								target: loader.item
								value: control.document
								property: "document"
							}

							Layout.fillWidth: item ? item.Layout.fillWidth : false
							Layout.fillHeight: item ? item.Layout.fillHeight : false
						}
					}
				}
				Layout.fillWidth: true
				Layout.margins: Kirigami.Units.largeSpacing
			}
			QQC2.Button {
				text: i18n("Done")
				onClicked: melonApp.editMode = false
				Layout.alignment: Qt.AlignRight
			}
		}
	}

	contentItem: RowLayout {
		Repeater {
			model: melonApp.toolbar.items
			delegate: Loader {
				id: loader

				required property var modelData
				required property int index
				sourceComponent: modelData.itemComponent
				onItemChanged: {
					maus.parent = loader.item
					maus.anchors.fill = loader.item
				}
				BlockMouseArea {
					id: maus
					enabled: melonApp.editMode
				}

				DragHandler {
					target: null
					enabled: melonApp.editMode
					onActiveChanged: {
						if (active) {
							loader.item.grabToImage((result) => {
								melonApp.toolbar.removeItemAt(loader.index)
								loader.Drag.imageSource = result.url
								loader.Drag.active = active
							})
						} else {
							loader.Drag.active = active
						}
					}
				}
				Drag.dragType: Drag.Automatic
				Drag.mimeData: {
					"application/x-melon-toolbar-action": loader.modelData.itemKind
				}

				Binding {
					target: loader.item
					value: control.document
					property: "document"
				}

				Melon.Beacon {
					id: beforeBeacon

					controller: beaconController
					beaconID: "before" + loader.index
					dataCallback: () => ["before", loader.index]

					anchors.top: parent.top
					anchors.bottom: parent.bottom
					anchors.left: parent.left
					width: 1
				}
				Rectangle {
					anchors.top: parent.top
					anchors.bottom: parent.bottom
					anchors.left: parent.left
					width: 2
					visible: beaconController.activeBeaconID == beforeBeacon.beaconID
					color: Kirigami.Theme.highlightColor
					z: 500
				}
				Melon.Beacon {
					id: afterBeacon

					controller: beaconController
					beaconID: "after" + loader.index
					dataCallback: () => ["after", loader.index]

					anchors.top: parent.top
					anchors.bottom: parent.bottom
					anchors.right: parent.right
					width: 1
				}
				Rectangle {
					anchors.top: parent.top
					anchors.bottom: parent.bottom
					anchors.right: parent.right
					width: 2
					visible: beaconController.activeBeaconID == afterBeacon.beaconID
					color: Kirigami.Theme.highlightColor
					z: 500
				}

				Layout.fillWidth: item ? item.Layout.fillWidth : false
				Layout.fillHeight: item ? item.Layout.fillHeight : false
			}
		}
	}
}