import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon
import QtGraphicalEffects 1.12
import QtQml.Models 2.15

Kirigami.ShadowedRectangle {
	color: Kirigami.Theme.backgroundColor
	border.width: 1
	border.color: Kirigami.ColorUtils.tintWithAlpha(color, Kirigami.Theme.textColor, 0.2)
	radius: 3
	shadow {
		size: Kirigami.Units.gridUnit
		color: Qt.rgba(0, 0, 0, 0.2)
		yOffset: 2
	}
}