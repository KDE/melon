// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.melon as Melon

QQC2.ToolButton {
	icon.width: Kirigami.Units.iconSizes.small
	icon.height: Kirigami.Units.iconSizes.small
	Layout.preferredWidth: Kirigami.Units.iconSizes.small + Kirigami.Units.largeSpacing
	Layout.preferredHeight: Kirigami.Units.iconSizes.small + Kirigami.Units.largeSpacing
}