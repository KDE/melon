#include <QQmlEngine>
#include <QQmlComponent>
#include <QFile>
#include <QQmlComponent>
#include "app.h"

struct SPApp::Private
{
	QQmlEngine *engine = nullptr;
	QQmlComponent *windowComponent = nullptr;
};

SPApp::SPApp(QObject *parent)
	: QObject(parent), d(new Private)
{
	d->engine = new QQmlEngine(this);
	d->windowComponent = new QQmlComponent(d->engine);

	QUrl windowQml("qrc:/Window.qml");
	QFile windowFile(":/Window.qml");
	Q_ASSERT(windowFile.open(QFile::ReadOnly));
	const auto windowData = windowFile.readAll();
	d->windowComponent->setData(windowData, windowQml);
};

SPApp::~SPApp()
{
}

void SPApp::ShowFile(const QString &uri, const QString &parentWindowHandle)
{
	qWarning() << "I'm being asked to show" << uri << "with parent window" << parentWindowHandle;
}
