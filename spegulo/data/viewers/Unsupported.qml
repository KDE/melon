import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.spegulo 1.0 as Spegulo

Item {
	id: _viewer

	required property Spegulo.Document document

	Kirigami.PlaceholderMessage {
		anchors.fill: parent
		text: "oh no i dont know how to view that :("
	}
}
