Product {
	name: "application-data"

	Group {
		files: ["org.kde.Delfenoj.svg"]
		qbs.install: true
		qbs.installDir: "share/icons/hicolor/scalable/apps"
	}
	Group {
		files: ["org.kde.Delfenoj.desktop"]
		qbs.install: true
		qbs.installDir: "share/applications"
	}
}
