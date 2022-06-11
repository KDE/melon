import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon
import QtGraphicalEffects 1.12
import QtQml.Models 2.15
import org.kde.qqc2desktopstyle.private 1.0 as QQC2DesktopStyle

Rectangle {
    required property Item target
    required visible

    parent: tryit(() => target.QQC2.ApplicationWindow.contentItem, null)

    z: 50000
    color: "transparent"
    border.color: Kirigami.Theme.highlightColor
    border.width: 2

    function tryit(closure, def) {
        try {
            return closure()
        } catch (error) {
            return def
        }
    }

    x: tryit(() => target.Kirigami.ScenePosition.x, 0)
    y: tryit(() => target.Kirigami.ScenePosition.y - target.QQC2.ApplicationWindow.header.height, 0)
    width: tryit(() => target.width, 0)
    height: tryit(() => target.height, 0)
}
