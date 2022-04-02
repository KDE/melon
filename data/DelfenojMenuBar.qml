import QtQuick 2.15
import Qt.labs.platform 1.1

MenuBar {
	Menu {
		title: i18n("Delfenoj")

		MenuItem {
			text: i18n("About Delfenoj")
		}

		MenuSeparator { }

		MenuItem {
			text: i18n("Preferences")
			shortcut: StandardKey.Preferences
		}

		MenuSeparator { }

		MenuItem {
			text: i18n("Empty Trash")
			shortcut: "Ctrl+Shift+Del"
		}
	}
	Menu {
		title: i18n("File")

		MenuItem {
			text: i18n("New Window")
			shortcut: "Ctrl+N"
		}
		MenuItem {
			text: i18n("New Folder")
			shortcut: "Ctrl+Shift+N"
		}
		MenuItem {
			text: i18n("Open")
			shortcut: StandardKey.Open
		}
		MenuItem {
			text: i18n("Close Window")
			shortcut: StandardKey.Close
		}

		MenuSeparator { }

		MenuItem {
			text: i18n("Duplicate")
			shortcut: "Ctrl+D"
		}
		MenuItem {
			text: i18n("Make Alias")
			shortcut: "Ctrl+L"
		}

		MenuSeparator { }

		MenuItem {
			text: i18n("Move To Trash")
			shortcut: "Ctrl+Del"
		}
	}
	Menu {
		title: i18n("Edit")

		MenuItem {
			text: i18n("Undo")
			shortcut: StandardKey.Undo
		}
		MenuItem {
			text: i18n("Redo")
			shortcut: StandardKey.Redo
		}

		MenuSeparator { }

		MenuItem {
			text: i18n("Cut")
			shortcut: StandardKey.Cut
		}
		MenuItem {
			text: i18n("Copy")
			shortcut: StandardKey.Copy
		}
		MenuItem {
			text: i18n("Paste")
			shortcut: StandardKey.Paste
		}
		MenuItem {
			text: i18n("Select All")
			shortcut: StandardKey.SelectAll
		}
	}
	Menu {
		title: i18n("View")

		MenuItem {
			text: i18n("as Icons")
		}
		MenuItem {
			text: i18n("as List")
		}

		MenuSeparator { }

		MenuItem {
			text: i18n("Show Path Bar")
		}
		MenuItem {
			text: i18n("Show Status Bar")
		}
		MenuItem {
			text: i18n("Show Sidebar")
		}

		MenuSeparator { }

		MenuItem {
			text: i18n("Hide Toolbar")
		}
		MenuItem {
			text: i18n("Customise Toolbar...")
		}
	}
	Menu {
		title: i18n("Go")

		MenuItem {
			text: i18n("Back")
		}
		MenuItem {
			text: i18n("Forward")
		}
		MenuItem {
			text: i18n("Containing Folder")
		}

		MenuSeparator { }

		// places
	}
}
