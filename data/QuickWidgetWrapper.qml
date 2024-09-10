import QtQuick
import QtQuick.Controls as QQC2

Item {
	id: _wrapper
	property Item child
	onChildChanged: {
		child.anchors.fill = _wrapper
	}
}