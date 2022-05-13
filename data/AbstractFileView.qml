import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon
import QtGraphicalEffects 1.12
import QtQml.Models 2.15
import org.kde.qqc2desktopstyle.private 1.0 as QQC2DesktopStyle

QQC2.Page {
	id: page

	required property Melon.Document document
	required property int itemCount

	header: ColumnLayout {
		QQC2.ToolBar {
			visible: page.document.searchBarOpen

			contentItem: ColumnLayout {
				spacing: 2

				Kirigami.SearchField {
					text: page.document.searchText
					onTextChanged: page.document.searchText = text
					Layout.fillWidth: true
				}
				RowLayout {
					spacing: 2

					QQC2.Label {
						text: i18n("Search through:")
						font: Kirigami.Theme.smallFont
					}
					QQC2.ToolButton {
						text: i18n("This Computer")
						down: !page.document.onlySearchingFromCurrentWorkingDirectory || pressed
						onClicked: page.document.onlySearchingFromCurrentWorkingDirectory = false
					}
					QQC2.ToolButton {
						text: i18n(`"%1"`, page.document.fancyNameFor(page.document.navigator.currentLocationUrl))
						down: page.document.onlySearchingFromCurrentWorkingDirectory || pressed
						onClicked: page.document.onlySearchingFromCurrentWorkingDirectory = true
					}
					Kirigami.Separator {
						Layout.fillHeight: true
					}
					QQC2.ToolButton {
						text: i18n("Contents")
						down: page.document.searchIncludesContents || pressed
						onClicked: page.document.searchIncludesContents = true
					}
					QQC2.ToolButton {
						text: i18n("File Name")
						down: !page.document.searchIncludesContents || pressed
						onClicked: page.document.searchIncludesContents = false
					}
					Item {
						Layout.fillWidth: true
					}
					QQC2.ToolButton {
						text: i18n("Save")
						onClicked: page.document.saveCurrentSearch()
					}
				}
			}

			Layout.fillWidth: true
		}
	}
	footer: PathBar {
		visible: melonApp.showPathBar
		document: page.document
	}

	DropArea {
		anchors.fill: parent
		onDropped: (event) => page.document.drop(page, event)
	}

	Kirigami.PlaceholderMessage {
		text: melonApp.kaomoji(`¯\\_(ツ)_/¯`)
		explanation: i18n("There are no objects in this directory")
		visible: page.itemCount === 0 && !page.document.loading

		anchors.centerIn: parent
	}
}