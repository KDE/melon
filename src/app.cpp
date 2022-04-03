#include <KLocalizedContext>
#include <QQmlContext>
#include <KWindowSystem>

#include "app.h"
#include "dbus.h"
#include "document.h"
#include "window.h"

SApp::SApp()
{
	instance = this;

	qRegisterMetaType<QList<SDocument*>>();

	filePlacesModel = new KFilePlacesModel(this);
	engine.reset(new QQmlEngine);
	engine->rootContext()->setContextProperty("delfenojApp", this);
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
	new SOrgFreedesktopFilemanager1(this);
}

SApp::~SApp()
{

}

SApp* SApp::instance;

void
SApp::start()
{
	newWindow();
}

void
SApp::newWindow()
{
	auto win = qobject_cast<QQuickWindow*>(windowComponent->beginCreate(engine->rootContext()));
	qWarning().noquote() << windowComponent->errorString();

	auto window = new SWindow(win, engine.get());

	windowComponent->setInitialProperties(win, {{"window", QVariant::fromValue(window)}});
	windowComponent->completeCreate();

	windows << window;
	KWindowSystem::activateWindow(window->displayedIn());
}

void
SApp::newWindowAtUrl(const QUrl &url)
{
	auto win = qobject_cast<QQuickWindow*>(windowComponent->beginCreate(engine->rootContext()));
	qWarning().noquote() << windowComponent->errorString();

	auto window = new SWindow(url.adjusted(QUrl::RemoveFilename), win, engine.get());

	windowComponent->setInitialProperties(win, {{"window", QVariant::fromValue(window)}});
	windowComponent->completeCreate();

	windows << window;
	KWindowSystem::activateWindow(window->displayedIn());
}

void
SApp::ensureShown(const QUrl &url)
{
	for (auto* window : windows) {
		auto docs = window->documents();
		for (auto* doc : docs) {
			if (doc->navigator()->currentLocationUrl() == url ||
				doc->navigator()->currentLocationUrl().adjusted(QUrl::RemoveFilename) == url.adjusted(QUrl::RemoveFilename)) {

				KWindowSystem::activateWindow(window->displayedIn());
				return;
			}
		}
	}
	newWindowAtUrl(url);
}

KFilePlacesModel*
SApp::placesModel() const
{
	return filePlacesModel;
}
