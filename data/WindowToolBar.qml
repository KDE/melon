import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj

QQC2.ToolBar {
	id: control

    position: QQC2.ToolBar.Footer

	required property Delfenoj.Window window
    required property int currentIndex

    readonly property Delfenoj.Document document: control.window.documents[control.currentIndex]

	contentItem: RowLayout {
        Kirigami.Icon {
            source: "emblem-readonly"
            visible: !control.document.writable
            Layout.preferredWidth: Kirigami.Units.iconSizes.small
            Layout.preferredHeight: Kirigami.Units.iconSizes.small
            Layout.alignment: Qt.AlignVCenter
        }
        Kirigami.Icon {
            source: "emblem-mounted"
            visible: !control.document.local
            Layout.preferredWidth: Kirigami.Units.iconSizes.small
            Layout.preferredHeight: Kirigami.Units.iconSizes.small
            Layout.alignment: Qt.AlignVCenter
        }
        QQC2.Label {
            text: i18n("%1 File(s), %2 Folder(s), %3", control.document.numberOfFiles, control.document.numberOfFolders, control.document.dirSize)
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }
        Delfenoj.What {

        }
	}
}