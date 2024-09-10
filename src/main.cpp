// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QQmlEngine>
#include <KLocalizedString>
#include <qwindow.h>

#include "app.h"
#include "foreigns.h"
#include "NGAppMain.h"

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
	auto appData = NGAppData{
		.translationDomain = "org.kde.melon",
		.desktopFile = u"org.kde.Melon"_s,
		.aboutData = KAboutData(),
		.initialisedCallback = [] { sApp->start(); },
	};
	return NGAppMain(&argc, &argv, appData);
}
