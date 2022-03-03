import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj

Window {
	id: window

	visible: true
	title: i18n("%1 - Delfenoj", document.title)

	required property Delfenoj.Document document

	Page {
		id: page

		anchors.fill: parent
		document: window.document
	}
}
