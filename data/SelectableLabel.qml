import QtQuick
import org.kde.kirigami as Kirigami

TextEdit {
    selectByMouse: true
    readOnly: true
    wrapMode: TextEdit.Wrap
    color: Kirigami.Theme.textColor
    selectedTextColor: Kirigami.Theme.highlightedTextColor
    selectionColor: Kirigami.Theme.highlightColor
}
