#include <QApplication>
#include <QQmlEngine>
#include <KLocalizedString>

#include "app.h"
#include "foreigns.h"
#include "nglib/NGAppMain.h"

int main(int argc, char *argv[])
{
	auto appData = NGAppData{
		.translationDomain = "org.kde.delfenoj",
		.desktopFile = "org.kde.Delfenoj",
		.aboutData = KAboutData(),
		.initialisedCallback = [] { sApp->start(); },
	};
	return NGAppMain(&argc, &argv, appData);
}
