#pragma once

#include <QApplication>
#include <QString>

#include <KAboutData>
#include <KConfigGroup>

#include <functional>

struct NGAppData
{
	const char* translationDomain;
	QString desktopFile;
	KAboutData aboutData;
	std::function<void()> initialisedCallback;
};

extern int NGAppMain(int* argc, char*** argv, const NGAppData& = NGAppData());
