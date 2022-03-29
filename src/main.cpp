#include <QApplication>
#include <QQmlEngine>
#include <KLocalizedString>

#include "app.h"
#include "foreigns.h"

#undef signals
#undef signal
#undef emit
#include <gtk/gtk.h>

int gargc;
char **gargv;

static void activate(GtkApplication* gapp, gpointer user_data)
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	KLocalizedString::setApplicationDomain("org.kde.delfenoj");

	QApplication app(gargc, gargv);
	app.setDesktopFileName("org.kde.Delfenoj");
	SApp sApp;

	sApp.start();
	app.exec();
}

int main(int argc, char *argv[])
{
	gargc = argc;
	gargv = argv;

	GtkApplication* gapp = gtk_application_new ("org.kde.Delfenoj", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(gapp, "activate", G_CALLBACK(activate), nullptr);
	g_application_run((GApplication*)gapp, argc, argv);

	return 0;
}
