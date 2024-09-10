// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon
import Qt5Compat.GraphicalEffects
import QtQml.Models

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
