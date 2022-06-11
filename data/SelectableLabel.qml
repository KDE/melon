import QtQuick 2.15
import org.kde.kirigami 2.15 as Kirigami

TextEdit {
    selectByMouse: true
    readOnly: true
    wrapMode: TextEdit.Wrap
    color: Kirigami.Theme.textColor
    selectedTextColor: Kirigami.Theme.highlightedTextColor
    selectionColor: Kirigami.Theme.highlightColor
}
