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

	Group {
		files: ["../data/**"]
		fileTags: "qt.core.resource_data"
		Qt.core.resourceSourceBase: "../data/"
		Qt.core.resourcePrefix: "/"
	}

	Qt.qml.importName: "org.kde.spegulo"
	Qt.qml.importVersion: "1.0"
	Depends { name: "Qt"; submodules: ["core", "gui", "gui-private", "widgets", "quick", "quick-private", "dbus", "quickcontrols2", "qml", "sql", "concurrent"] }
}