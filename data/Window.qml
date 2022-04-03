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

	header: ColumnLayout {
		width: parent && parent.width
		spacing: 0

		LayoutMirroring.childrenInherit: Qt.application.layoutDirection == Qt.RightToLeft
		LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft

		WindowTabBar {
			id: tabBar
			window: window.window

			Layout.fillWidth: true
		}
		PageToolBar {
			document: window.window.documents[tabBar.currentIndex]

			Layout.fillWidth: true
		}
	}
	footer: WindowToolBar {
		window: window.window
		currentIndex: tabBar.currentIndex

		LayoutMirroring.childrenInherit: Qt.application.layoutDirection == Qt.RightToLeft
		LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
	}

	RowLayout {
		spacing: 0
		anchors.fill: parent

		LayoutMirroring.childrenInherit: Qt.application.layoutDirection == Qt.RightToLeft
		LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft

		PlacesSidebar {
			document: window.window.documents[tabBar.currentIndex]
		}

		Kirigami.Separator {
			Layout.fillHeight: true
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
