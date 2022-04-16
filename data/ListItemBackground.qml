import QtQuick 2.1
import org.kde.kirigami 2.15 as Kirigami

Rectangle {
	id: background

	required property bool pressed
	required property bool hovered
	required property Item item

	color: pressed
		? Kirigami.Theme.highlightColor
		: Kirigami.Theme.backgroundColor

	Rectangle {
		id: internal
		anchors.fill: parent
		visible: !Kirigami.Settings.tabletMode
		color: Kirigami.Theme.highlightColor
		opacity: background.hovered && !background.pressed ? 0.5 : 0
	}

	Kirigami.Separator {
		anchors {
			left: parent.left
			right: parent.right
			bottom: parent.bottom
			leftMargin: Kirigami.Units.largeSpacing
			rightMargin: Kirigami.Units.largeSpacing
		}
		visible: !background.pressed
			&& (!background.hovered)
			&& (!!background.item.ListView.view ? background.item.ListView.nextSection == background.item.ListView.section : true)
		weight: Kirigami.Separator.Weight.Light
	}
}

