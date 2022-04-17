import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.delfenoj 1.0 as Delfenoj

QQC2.ToolButton {
	icon.width: Kirigami.Units.iconSizes.small
	icon.height: Kirigami.Units.iconSizes.small
	Layout.preferredWidth: Kirigami.Units.iconSizes.small + Kirigami.Units.largeSpacing
	Layout.preferredHeight: Kirigami.Units.iconSizes.small + Kirigami.Units.largeSpacing
}