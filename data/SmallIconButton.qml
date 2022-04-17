import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon

QQC2.ToolButton {
	icon.width: Kirigami.Units.iconSizes.small
	icon.height: Kirigami.Units.iconSizes.small
	Layout.preferredWidth: Kirigami.Units.iconSizes.small + Kirigami.Units.largeSpacing
	Layout.preferredHeight: Kirigami.Units.iconSizes.small + Kirigami.Units.largeSpacing
}