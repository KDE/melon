import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.spegulo 1.0 as Spegulo
import org.kde.kirigami 2.15 as Kirigami

Item {
	id: _viewer

	required property Spegulo.Document document
	required property string text
	readonly property var textDocument: _textViewer.textDocument

	QQC2.ScrollView {
		anchors.fill: parent

		Flickable {
			contentWidth: _textViewer.implicitWidth
			contentHeight: _textViewer.implicitHeight

			TextEdit {
				id: _textViewer
				text: _viewer.text

				readOnly: true
				selectByMouse: !Kirigami.Settings.isMobile
				wrapMode: Text.Wrap

				color: Kirigami.Theme.textColor
				selectedTextColor: Kirigami.Theme.highlightedTextColor
				selectionColor: Kirigami.Theme.highlightColor
			}
		}
	}
}
