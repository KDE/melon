Product {
	name: "application-data"

	Group {
		files: ["org.kde.Melon.svg"]
		qbs.install: true
		qbs.installDir: "share/icons/hicolor/scalable/apps"
	}
	Group {
		files: ["org.kde.Melon.desktop"]
		qbs.install: true
		qbs.installDir: "share/applications"
	}
}
