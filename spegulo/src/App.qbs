import qbs.Process

QtApplication {
	name: "org.kde.Spegulo"
	files: [
		"*.cpp",
		"*.h",
	]
	cpp.cxxLanguageVersion: "c++17"
	install: true

	Group {
		files: "org.kde.Spegulo.xml"
		fileTags: "qt.dbus.adaptor"
	}

	Qt.qml.importName: "org.kde.spegulo"
	Qt.qml.importVersion: "1.0"
	Depends { name: "Qt"; submodules: ["core", "gui", "gui-private", "widgets", "quick", "quick-private", "quickcontrols2", "qml", "sql", "concurrent"] }
}