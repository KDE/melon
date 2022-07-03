import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.spegulo 1.0 as Spegulo

QQC2.ApplicationWindow {
	id: window

	required property Spegulo.Document document
	property Item innerItem: null
	onInnerItemChanged: {
		innerItem.parent = this.contentItem
		innerItem.anchors.fill = innerItem.parent
	}

	title: document.title

	header: WindowHeader {
		document: window.document
	}

	visible: true
}
