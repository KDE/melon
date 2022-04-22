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