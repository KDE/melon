import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon

QQC2.ApplicationWindow {
	id: window

	visible: true
	title: i18n("%1 - Melon", window.window.documents[tabBar.currentIndex].title)

	required property Melon.Window window

	width: Kirigami.Units.gridUnit * 40
	height: Kirigami.Units.gridUnit * 20

	Binding {
		target: window.window
		property: "activeDocument"
		value: window.window.documents[tabBar.currentIndex]
	}

	header: ColumnLayout {
		width: parent && parent.width
		spacing: 0

		LayoutMirroring.childrenInherit: Qt.application.layoutDirection == Qt.RightToLeft
		LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft

		PageToolBar {
			visible: melonApp.showToolbar
			outlet: window.window.documents[tabBar.currentIndex]

			Layout.fillWidth: true
		}
		Kirigami.Separator {
			visible: !melonApp.showToolbar
			Layout.fillWidth: true
		}
	}
	footer: WindowToolBar {
		window: window.window
		currentIndex: tabBar.currentIndex
		visible: melonApp.showStatusBar

		LayoutMirroring.childrenInherit: Qt.application.layoutDirection == Qt.RightToLeft
		LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
	}

	RowLayout {
		spacing: 0
		anchors.fill: parent

		LayoutMirroring.childrenInherit: Qt.application.layoutDirection == Qt.RightToLeft
		LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft

		PlacesSidebar {
			visible: melonApp.showSidebar
			document: window.window.documents[tabBar.currentIndex]
		}

		Kirigami.Separator {
			visible: melonApp.showSidebar
			Layout.fillHeight: true
		}

		ColumnLayout {
			spacing: 0

			Layout.fillWidth: true
			Layout.fillHeight: true

			WindowTabBar {
				id: tabBar
				window: window.window
				visible: window.window.documents.length > 1

				Layout.fillWidth: true
			}

			StackLayout {
				currentIndex: tabBar.currentIndex

				Layout.fillWidth: true
				Layout.fillHeight: true

				Repeater {
					model: window.window.documents
					delegate: Loader {
						id: loader
						required property Melon.Document modelData

						sourceComponent: {
							if (melonApp.viewMode === Melon.App.Columns) {
								switch (loader.modelData.searchBarOpen) {
								case true: return tableFileView
								case false: return columnsFileView
								default: return columnsFileView
								}
							}
							const kind = Melon.FolderClassifier.classifyFolder(loader.modelData.actualViewingURL)
							switch (kind) {
							case "photos":
								return photosFileView
							default:
								switch (melonApp.viewMode) {
								case Melon.App.Icons: return iconsFileView
								case Melon.App.List: return tableFileView
								default: return tableFileView
								}
							}
						}
					}
				}
				Component { id: iconsFileView; IconsFileView { document: modelData } }
				Component { id: photosFileView; PhotosFileView { document: modelData } }
				Component { id: tableFileView; TableFileView { document: modelData } }
				Component { id: columnsFileView; ColumnsFileView { document: modelData } }
			}
		}
	}
}
