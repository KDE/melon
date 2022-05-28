#include <KLocalizedContext>
#include <QQmlContext>
#include <KWindowSystem>
#include <KAboutData>
#include <QFile>
#include <QDir>
#include <KSharedConfig>
#include <QUuid>
#include <QGuiApplication>
#include <QMenu>
#include <KLocalizedString>

#include "app.h"
#include "conversioncheck.h"
#include "dbus.h"
#include "document.h"
#include "qguiapplication.h"
#include "window.h"
#include "toolbar.h"

struct SApp::Private
{
	bool showSidebar = true;
	bool showStatusBar = true;
	bool showToolbar = true;
	bool showPathBar = true;
	bool uuumauma = false;
	bool iconsView = false;
	bool editMode = false;
	AppMode appMode = AppMode::Spatial;
	KSharedConfigPtr config;
	SToolBar* toolbar = nullptr;
};


SApp* SApp::instance()
{
	static SApp* sAppInstance = nullptr;
	if (sAppInstance == nullptr) {
		sAppInstance = (SApp*)malloc(sizeof(SApp));
		new (sAppInstance) SApp;
	}
	return sAppInstance;
}

SApp::SApp() : QObject(), d(new Private)
{
	d->config = KSharedConfig::openConfig();
	d->uuumauma = QFile::exists(QDir::homePath() + QDir::separator() + ".ｳｯｰｳｯｰｳﾏｳﾏ");

	qRegisterMetaType<QList<SDocument*>>();

	filePlacesModel = new KFilePlacesModel(this);
	sMenuBar = new SMenuBar(this);
	engine.reset(new QQmlEngine);
	engine->rootContext()->setContextProperty("melonApp", this);
	engine->rootContext()->setContextObject(new KLocalizedContext(engine.get()));
	windowComponent.reset(new QQmlComponent(engine.get()));
	aboutComponent.reset(new QQmlComponent(engine.get()));
	d->toolbar = new SToolBar(this);

	QUrl windowQml("qrc:/Window.qml");
	QFile windowFile(":/Window.qml");
	Q_ASSERT(windowFile.open(QFile::ReadOnly));
	const auto windowData = windowFile.readAll();
	windowComponent->setData(windowData, windowQml);

	QUrl aboutQml("qrc:/AboutDialog.qml");
	QFile aboutFile(":/AboutDialog.qml");
	Q_ASSERT(aboutFile.open(QFile::ReadOnly));
	const auto aboutData = aboutFile.readAll();
	aboutComponent->setData(aboutData, aboutQml);

	new SOrgFreedesktopFilemanager1(this);
}

SApp::~SApp()
{
	qWarning() << "self destruct";
}

void SApp::start()
{
	if (windows.isEmpty())
		newWindow();
}

SToolBar* SApp::toolbar() const
{
	return d->toolbar;
}

// void SApp::load()
// {
// 	for (const auto& group : d->config->groupList()) {
// 		if (!group.startsWith("window-"))
// 			continue;

// 		auto subgroup = d->config->group(group);

// 		auto win = qobject_cast<QQuickWindow*>(windowComponent->beginCreate(engine->rootContext()));
// 		qWarning().noquote() << windowComponent->errorString();

// 		auto window = new SWindow(subgroup, win, engine.get());

// 		windowComponent->setInitialProperties(win, {{"window", QVariant::fromValue(window)}});
// 		windowComponent->completeCreate();

// 		window->afterComponentComplete(subgroup);

// 		windows << window;
// 		connect(window, &SWindow::closing, this, &SApp::windowClosing);
// 	}
// }

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

	auto window = new SWindow(url.adjusted(QUrl::StripTrailingSlash), win, engine.get());

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
				doc->navigator()->currentLocationUrl().adjusted(QUrl::StripTrailingSlash) == url.adjusted(QUrl::StripTrailingSlash)) {

				KWindowSystem::activateWindow(window->displayedIn());
				return;
			}
		}
	}
	qWarning() << "making new window at url" << url;
	newWindowAtUrl(url);
}

SApp::AppMode SApp::appMode() const
{
	return d->appMode;
}

void SApp::setAppMode(AppMode mode)
{
	if (d->appMode == mode)
		return;

	d->appMode = mode;
	Q_EMIT appModeChanged();
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

bool SApp::iconsView() const
{
	return d->iconsView;
}

void SApp::setIconsView(bool enabled)
{
	if (d->iconsView == enabled)
		return;

	d->iconsView = enabled;
	Q_EMIT iconsViewChanged();
}

void SApp::openRightClickMenuForPlace(const QModelIndex& idx)
{
	QPointer<QMenu> menu(new QMenu);
	auto url = filePlacesModel->url(idx);

	menu->addAction(QIcon::fromTheme("tab-new"), i18n("Open In New Tab"), [=] {
		auto window = QGuiApplication::focusWindow();
		auto swindow = sApp->swindowForWindow(window);

		swindow->newDocumentAtUrl(url);
	});
	menu->addAction(QIcon::fromTheme("window-new"), i18n("Open In New Window"), [=] {
		newWindowAtUrl(url);
	});
	menu->addSeparator();

	auto action = filePlacesModel->ejectActionForIndex(idx);
	if (action) menu->addAction(action);
	action = filePlacesModel->teardownActionForIndex(idx);
	if (action) menu->addAction(action);

	menu->createWinId();
	menu->windowHandle()->setTransientParent(qGuiApp->focusWindow());
	menu->exec(QCursor::pos());

	if (menu) {
		menu->deleteLater();
	}
}

bool SApp::editMode() const
{
	return d->editMode;
}

void SApp::setEditMode(bool editMode)
{
	if (d->editMode == editMode)
		return;

	d->editMode = editMode;
	Q_EMIT editModeChanged();
}
