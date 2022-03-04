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

	header: WindowTabBar {
		id: tabBar
		window: window.window
	}

	StackLayout {
		currentIndex: tabBar.currentIndex
		anchors.fill: parent

		Repeater {
			model: window.window.documents
			delegate: Page {
				required property Delfenoj.Document modelData

				document: modelData
			}
		}
	}
}
