#include <QApplication>
#include <QQmlEngine>

#include "app.h"
#include "foreigns.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QApplication app(argc, argv);
	SApp sApp;

	sApp.start();

	return QCoreApplication::exec();
}
