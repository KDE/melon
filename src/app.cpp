#include <KLocalizedContext>
#include <QQmlContext>
#include <KWindowSystem>
#include <KAboutData>
#include <QFile>
#include <QDir>
#include <KSharedConfig>
#include <QUuid>
#include <QGuiApplication>

#include "app.h"
#include "conversioncheck.h"
#include "dbus.h"
#include "document.h"
#include "window.h"

struct SApp::Private
{
	bool showSidebar = true;
	bool showStatusBar = true;
	bool showToolbar = true;
	bool showPathBar = true;
	bool uuumauma = false;
	KSharedConfigPtr config;
};

SApp::SApp() : QObject(), d(new Private)
{
	d->config = KSharedConfig::openConfig();
	d->uuumauma = QFile::exists(QDir::homePath() + QDir::separator() + ".ｳｯｰｳｯｰｳﾏｳﾏ");
	instance = this;

	qRegisterMetaType<QList<SDocument*>>();

	filePlacesModel = new KFilePlacesModel(this);
	sMenuBar = new SMenuBar(this);
	engine.reset(new QQmlEngine);
	engine->rootContext()->setContextProperty("delfenojApp", this);
	engine->rootContext()->setContextObject(new KLocalizedContext(engine.get()));
	windowComponent.reset(new QQmlComponent(engine.get()));
	pageComponent.reset(new QQmlComponent(engine.get()));
	aboutComponent.reset(new QQmlComponent(engine.get()));

	QUrl windowQml("qrc:/Window.qml");
	QFile windowFile(":/Window.qml");
	Q_ASSERT(windowFile.open(QFile::ReadOnly));
	const auto windowData = windowFile.readAll();
	windowComponent->setData(windowData, windowQml);

	QUrl pageQml("qrc:/Page.qml");
	QFile pageFile(":/Page.qml");
	Q_ASSERT(pageFile.open(QFile::ReadOnly));
	const auto pageData = pageFile.readAll();
	pageComponent->setData(pageData, pageQml);

	QUrl aboutQml("qrc:/AboutDialog.qml");
	QFile aboutFile(":/AboutDialog.qml");
	Q_ASSERT(aboutFile.open(QFile::ReadOnly));
	const auto aboutData = aboutFile.readAll();
	aboutComponent->setData(aboutData, aboutQml);

	connect(qApp, &QGuiApplication::aboutToQuit, this, &SApp::save);

	new SOrgFreedesktopFilemanager1(this);
}

SApp::~SApp()
{
}

SApp* SApp::instance;

void SApp::start()
{
	bool hasConfig = false;
	for (const auto& group : d->config->groupList())
		if (group.startsWith("window-")) {
			hasConfig = true;
			break;
		}

	if (hasConfig)
		load();
	else
		newWindow();
}

void SApp::load()
{
	for (const auto& group : d->config->groupList()) {
		if (!group.startsWith("window-"))
			continue;

		auto subgroup = d->config->group(group);

		auto win = qobject_cast<QQuickWindow*>(windowComponent->beginCreate(engine->rootContext()));
		qWarning().noquote() << windowComponent->errorString();

		auto window = new SWindow(subgroup, win, engine.get());

		windowComponent->setInitialProperties(win, {{"window", QVariant::fromValue(window)}});
		windowComponent->completeCreate();

		window->afterComponentComplete(subgroup);

		windows << window;
		connect(window, &SWindow::closing, this, &SApp::windowClosing);
	}
}

void SApp::save()
{
	for (const auto& group : d->config->groupList())
		if (group.startsWith("window-"))
			d->config->deleteGroup(group);

	for (const auto* window : windows) {
		auto subgroup = d->config->group("window-" + window->id().toString(QUuid::WithoutBraces));
		window->saveTo(subgroup);
	}
}

void SApp::newWindow()
{
	auto win = qobject_cast<QQuickWindow*>(windowComponent->beginCreate(engine->rootContext()));
	qWarning().noquote() << windowComponent->errorString();

	auto window = new SWindow(win, engine.get());

	windowComponent->setInitialProperties(win, {{"window", QVariant::fromValue(window)}});
	windowComponent->completeCreate();

	windows << window;
	connect(window, &SWindow::closing, this, &SApp::windowClosing);
	KWindowSystem::activateWindow(window->displayedIn());
}

void SApp::newWindowAtUrl(const QUrl& url)
{
	auto win = qobject_cast<QQuickWindow*>(windowComponent->beginCreate(engine->rootContext()));
	qWarning().noquote() << windowComponent->errorString();

	auto window = new SWindow(url.adjusted(QUrl::RemoveFilename), win, engine.get());

	windowComponent->setInitialProperties(win, {{"window", QVariant::fromValue(window)}});
	windowComponent->completeCreate();

	windows << window;
	connect(window, &SWindow::closing, this, &SApp::windowClosing);
	KWindowSystem::activateWindow(window->displayedIn());
}

void SApp::windowClosing(SWindow* window)
{
	windows.removeAll(window);
	window->deleteLater();
}

void SApp::ensureShown(const QUrl& url)
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

SWindow*
SApp::swindowForWindow(QWindow* window)
{
	for (auto w : windows) {
		if (w->displayedIn() == window) {
			return w;
		}
	}
	return nullptr;
}

bool SApp::showSidebar() const
{
	return d->showSidebar;
}

void SApp::setShowSidebar(bool show)
{
	if (d->showSidebar == show)
		return;

	d->showSidebar = show;
	Q_EMIT showSidebarChanged();
}

bool SApp::showStatusBar() const
{
	return d->showStatusBar;
}

void SApp::setShowStatusBar(bool show)
{
	if (d->showStatusBar == show)
		return;

	d->showStatusBar = show;
	Q_EMIT showStatusBarChanged();
}

bool SApp::showToolbar() const
{
	return d->showToolbar;
}

void SApp::setShowToolbar(bool show)
{
	if (d->showToolbar == show)
		return;

	d->showToolbar = show;
	Q_EMIT showToolbarChanged();
}

bool SApp::showPathBar() const
{
	return d->showPathBar;
}

void SApp::setShowPathBar(bool show)
{
	if (d->showPathBar == show)
		return;

	d->showPathBar = show;
	Q_EMIT showPathBarChanged();
}

QString SApp::kaomoji(const QString& str)
{
	if (d->uuumauma)
		return "(ﾟ∀ﾟ)";
	return str;
}
