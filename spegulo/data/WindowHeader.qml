import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.spegulo 1.0 as Spegulo
import QtQuick.Layouts 1.15

QQC2.ToolBar {
	id: toolbar

	required property Spegulo.Document document

	visible: true

	contentItem: RowLayout {
		Item { Layout.fillWidth: true }
		QQC2.Button {
			text: `Open With ${document.defaultApps[0].name}`
			icon.name: document.defaultApps[0].icon
		}
	}
}
