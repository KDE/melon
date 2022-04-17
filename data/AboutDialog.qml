import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami
import org.kde.melon 1.0 as Melon

// TODO port to kirigami about item
QQC2.ApplicationWindow {
    title: i18n("About Melon")

    flags: Qt.Dialog
    minimumWidth: cont.implicitWidth
    minimumHeight: cont.implicitHeight
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight

    background: Rectangle {
        Kirigami.Separator {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
        }
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        color: Kirigami.Theme.backgroundColor
    }
    QQC2.Control {
        id: cont
        horizontalPadding: Kirigami.Units.gridUnit*2
        leftPadding: horizontalPadding
        rightPadding: horizontalPadding
        verticalPadding: Kirigami.Units.gridUnit
        topPadding: verticalPadding
        bottomPadding: verticalPadding
        contentItem: ColumnLayout {
            Kirigami.Icon {
                source: "org.kde.Melon"

                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: Kirigami.Units.iconSizes.enormous
                Layout.preferredHeight: Kirigami.Units.iconSizes.enormous
            }

            QQC2.Label {
                text: i18n("Melon")
                font.bold: true
                horizontalAlignment: Text.AlignHCenter

                Layout.fillWidth: true
            }
            QQC2.Label {
                text: i18n("Simple file manager")
                horizontalAlignment: Text.AlignHCenter

                Layout.fillWidth: true
            }
            QQC2.Label {
                text: i18n("Melon Git Master Version")
                horizontalAlignment: Text.AlignHCenter
                font: Kirigami.Theme.smallFont

                Layout.fillWidth: true
            }
            Item { Layout.preferredHeight: Kirigami.Units.gridUnit }
            QQC2.Label {
                text: i18n("Copyright 2022 Janet Blackquill")
                horizontalAlignment: Text.AlignHCenter
                font: Kirigami.Theme.smallFont

                Layout.fillWidth: true
            }
        }
    }
}
