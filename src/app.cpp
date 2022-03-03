#include "app.h"
#include "document.h"

SApp::SApp()
{
	instance = this;

	engine.reset(new QQmlEngine);
	windowComponent.reset(new QQmlComponent(engine.get()));

	QUrl windowQml("qrc:/Window.qml");
	QFile it(":/Window.qml");
	Q_ASSERT(it.open(QFile::ReadOnly));
	const auto data = it.readAll();

	windowComponent->setData(data, windowQml);
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

	auto document = new SDocument(win, engine.get());

	windowComponent->setInitialProperties(win, {{"document", QVariant::fromValue(document)}});
	windowComponent->completeCreate();
}

