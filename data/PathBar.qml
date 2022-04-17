import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.melon 1.0 as Melon
import QtGraphicalEffects 1.12
import QtQml.Models 2.15
import org.kde.qqc2desktopstyle.private 1.0 as QQC2DesktopStyle

QQC2.ToolBar {
	id: bar

	required property Melon.Document document

	implicitHeight: topPadding + contentItem.implicitHeight + bottomPadding

	contentItem: RowLayout {
		spacing: Kirigami.Units.smallSpacing
		Repeater {
			model: bar.document.pathSegmentStrings
			delegate: RowLayout {
				id: del

				required property string modelData
				required property int index

				QQC2.Label {
					text: del.modelData
					font: Kirigami.Theme.smallFont
					verticalAlignment: Text.AlignVCenter

					Layout.fillHeight: true

					TapHandler {
						onDoubleTapped: bar.document.navigator.currentLocationUrl = bar.document.pathSegmentURLs[del.index]
					}
				}

				Kirigami.Icon {
					visible: del.index+1 != bar.document.pathSegmentStrings.length
					source: "arrow-right"

					Layout.alignment: Qt.AlignVCenter
					Layout.preferredWidth: Kirigami.Units.iconSizes.small
					Layout.preferredHeight: Kirigami.Units.iconSizes.small
				}
			}
		}
		Item { Layout.fillWidth: true }
	}
}