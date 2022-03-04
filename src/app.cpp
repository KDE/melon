#include <KLocalizedContext>
#include <QQmlContext>

#include "app.h"
#include "document.h"
#include "window.h"

SApp::SApp()
{
	instance = this;

	engine.reset(new QQmlEngine);
	engine->rootContext()->setContextObject(new KLocalizedContext(engine.get()));
	windowComponent.reset(new QQmlComponent(engine.get()));
	pageComponent.reset(new QQmlComponent(engine.get()));

	QUrl windowQml("qrc:/Window.qml");
	QFile windowFile(":/Window.qml");
	Q_ASSERT(windowFile.open(QFile::ReadOnly));
	const auto windowData = windowFile.readAll();
	windowComponent->setData(windowData, windowQml);

	QUrl pageQml("qrc:/Page.qml");
	QFile pageFile(":/Page.qml");
	Q_ASSERT(pageFile.open(QFile::ReadOnly));
	const auto pageData = pageFile.readAll();
	pageComponent->setData(pageData, windowQml);
}

SApp::~SApp()
{

}

SApp* SApp::instance;

void
SApp::start()
{
	newDocument();
}

void
SApp::newDocument()
{
	auto win = qobject_cast<QQuickWindow*>(windowComponent->beginCreate(engine->rootContext()));
	qWarning().noquote() << windowComponent->errorString();

	auto document = new SDocument(engine.get());
	auto window = new SWindow(win, document, engine.get());

	windowComponent->setInitialProperties(win, {{"window", QVariant::fromValue(window)}});
	windowComponent->completeCreate();
}

