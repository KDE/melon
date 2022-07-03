#include <QApplication>
#include <QDBusConnection>

#include "app.h"
#include "spegulo_adaptor.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	SPApp sapp;
	SpeguloAdaptor adaptor(&sapp);
	if (!QDBusConnection::sessionBus().registerObject("/", &sapp)) {
		qWarning() << "Failed to register adaptor:" << QDBusConnection::sessionBus().lastError().message();
		app.exit(-1);
	}

	if (!QDBusConnection::sessionBus().registerService("org.kde.Spegulo")) {
		qWarning() << "Failed to register service:" << QDBusConnection::sessionBus().lastError().message();
		app.exit(-1);
	}

	return app.exec();
}
