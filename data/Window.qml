import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj

QQC2.ApplicationWindow {
	id: window

	visible: true
	title: i18n("%1 - Delfenoj", window.window.documents[tabBar.currentIndex].title)

	required property Delfenoj.Window window

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
			visible: delfenojApp.showToolbar
			document: window.window.documents[tabBar.currentIndex]

			Layout.fillWidth: true
		}
		Kirigami.Separator {
			visible: !delfenojApp.showToolbar
			Layout.fillWidth: true
		}
	}
	footer: WindowToolBar {
		window: window.window
		currentIndex: tabBar.currentIndex
		visible: delfenojApp.showStatusBar

		LayoutMirroring.childrenInherit: Qt.application.layoutDirection == Qt.RightToLeft
		LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
	}

	RowLayout {
		spacing: 0
		anchors.fill: parent

		LayoutMirroring.childrenInherit: Qt.application.layoutDirection == Qt.RightToLeft
		LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft

		PlacesSidebar {
			visible: delfenojApp.showSidebar
			document: window.window.documents[tabBar.currentIndex]
		}

		Kirigami.Separator {
			visible: delfenojApp.showSidebar
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
					delegate: Page {
						required property Delfenoj.Document modelData

						document: modelData
					}
				}
			}
		}
	}
}
