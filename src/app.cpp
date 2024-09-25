// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <KBookmarkManager>
#include <QMainWindow>
#include <QQuickWidget>

#include "app.h"
#include "closesignalwindow.h"
#include "dbus.h"
#include "document.h"
#include "qguiapplication.h"
#include "toolbardelegate.h"
#include "window.h"
#include "NGLibQuick.h"

using namespace Qt::Literals::StringLiterals;

struct SApp::Private
{
	bool showSidebar = true;
	bool showStatusBar = true;
	bool showToolbar = true;
	bool showPathBar = true;
	bool uuumauma = false;
	SApp::ViewMode viewMode = SApp::List;
	bool editMode = false;
	KSharedConfigPtr config;
	NGToolBarController* toolbarController = nullptr;
	SToolBarDelegate* toolbarDelegate = nullptr;
	KBookmarkManager* bookmarkManager = nullptr;
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
	d->uuumauma = QFile::exists(QDir::homePath() + QDir::separator() + u".ｳｯｰｳｯｰｳﾏｳﾏ"_s);
	d->viewMode = (SApp::ViewMode)d->config->group(u"Settings"_s).readEntry("ViewMode", (int)ViewMode::List);

	qRegisterMetaType<QList<SDocument*>>();

	// try to find the kfile bookmarks...
	auto bookmarksPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"kfile/bookmarks.xml"_s);
	auto blankBookmarks = false;
	// or the dolphin bookmarks...
	if (bookmarksPath.isEmpty()) {
		bookmarksPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"dolphin/bookmarks.xml"_s);
	}
	// or our bookmarks
	if (bookmarksPath.isEmpty()) {
		bookmarksPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"melon/bookmarks.xml"_s);
	}
	// or if none of those are available, let's make our own
	if (bookmarksPath.isEmpty()) {
		const auto location = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
		const auto separator = QDir::separator();
		bookmarksPath = location + separator + u"melon"_s;
		QDir().mkpath(bookmarksPath);
		bookmarksPath += separator + QStringLiteral("bookmarks.xml");
		blankBookmarks = true;
	}
	d->bookmarkManager = new KBookmarkManager(u"Melon"_s, this);

	// if we're going from scratch, let's make some initial bookmarks...
	if (blankBookmarks) {
		d->bookmarkManager->root().addBookmark(
			i18nc("user's downloads folder", "Downloads"),
			QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)),
			u"folder-documents"_s);
		d->bookmarkManager->root().addBookmark(
			i18nc("user's documents folder", "Documents"),
			QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)),
			u"folder-documents"_s);
		d->bookmarkManager->root().addBookmark(
			i18nc("user's desktop folder", "Desktop"),
			QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)),
			u"desktop"_s);
		d->bookmarkManager->root().addBookmark(
			i18nc("user's home folder", "Home"),
			QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)),
			u"user-home"_s);
		d->bookmarkManager->root().addBookmark(
			i18nc("root folder", "Computer"),
			QUrl::fromLocalFile(QDir::rootPath()),
			u"folder-root"_s);
	}

	filePlacesModel = new KFilePlacesModel(this);
	sMenuBar = new SMenuBar(this);
	engine.reset(new QQmlEngine);
	engine->rootContext()->setContextProperty(u"melonApp"_s, this);
	engine->rootContext()->setContextObject(new KLocalizedContext(engine.get()));
	windowComponent.reset(new QQmlComponent(engine.get()));
	aboutComponent.reset(new QQmlComponent(engine.get()));
	d->toolbarDelegate = new SToolBarDelegate(engine.get());
	d->toolbarController = new NGToolBarController(u"main-toolbar"_s, d->toolbarDelegate, nullptr, this);

	QUrl windowQml(u"qrc:/Window.qml"_s);
	QFile windowFile(u":/Window.qml"_s);
	windowFile.open(QFile::ReadOnly);
	const auto windowData = windowFile.readAll();
	windowComponent->setData(windowData, windowQml);

	QUrl aboutQml(u"qrc:/AboutDialog.qml"_s);
	QFile aboutFile(u":/AboutDialog.qml"_s);
	aboutFile.open(QFile::ReadOnly);
	const auto aboutData = aboutFile.readAll();
	aboutComponent->setData(aboutData, aboutQml);

	new SOrgFreedesktopFilemanager1(this);
}

SApp::~SApp()
{
	qWarning() << "self destruct";
}

void SApp::activate(const QStringList &params, const QString &cwd)
{
	if (params.count() == 2) {
		ensureShown(QUrl::fromLocalFile(cwd).resolved(QUrl::fromLocalFile(params[1])));
	} else {
		if (windows.isEmpty()) {
			newWindow();
		} else {
			auto window = windows.first()->displayedIn()->windowHandle();
			window->show();
			KWindowSystem::updateStartupId(window);
			window->raise();
			KWindowSystem::activateWindow(window);
		}
	}
}

NGToolBarController* SApp::toolbarController() const
{
	return d->toolbarController;
}

KBookmarkManager* SApp::bookmarkManager() const
{
	return d->bookmarkManager;
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
	auto win = qobject_cast<QQuickItem*>(windowComponent->beginCreate(engine->rootContext()));
	qWarning().noquote() << windowComponent->errorString();

	auto closeWindow = new SCloseSignalWindow();
	closeWindow->init(win);

	auto window = new SWindow(closeWindow, engine.get());

	windowComponent->setInitialProperties(win, {{u"window"_s, QVariant::fromValue(window)}});
	windowComponent->completeCreate();

	closeWindow->show();
	KWindowSystem::updateStartupId(window->displayedIn()->windowHandle());

	windows << window;
	connect(window, &SWindow::closing, this, &SApp::windowClosing);
	window->displayedIn()->windowHandle()->raise();
	KWindowSystem::activateWindow(window->displayedIn()->windowHandle());
}

void SApp::newWindowAtUrl(const QUrl& url)
{
	auto win = qobject_cast<QQuickItem*>(windowComponent->beginCreate(engine->rootContext()));
	qWarning().noquote() << windowComponent->errorString();

	auto closeWindow = new SCloseSignalWindow();
	closeWindow->init(win);

	auto window = new SWindow(url, closeWindow, engine.get());

	closeWindow->show();
	KWindowSystem::updateStartupId(window->displayedIn()->windowHandle());

	windowComponent->setInitialProperties(win, {{u"window"_s, QVariant::fromValue(window)}});
	windowComponent->completeCreate();

	windows << window;
	connect(window, &SWindow::closing, this, &SApp::windowClosing);
	window->displayedIn()->windowHandle()->raise();
	KWindowSystem::activateWindow(window->displayedIn()->windowHandle());
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

				window->displayedIn()->windowHandle()->show();
				KWindowSystem::updateStartupId(window->displayedIn()->windowHandle());

				window->displayedIn()->windowHandle()->raise();
				KWindowSystem::activateWindow(window->displayedIn()->windowHandle());
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
		if (w->displayedIn()->windowHandle() == window) {
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
		return u"(ﾟ∀ﾟ)"_s;
	return str;
}

SApp::ViewMode SApp::viewMode() const
{
	return d->viewMode;
}

void SApp::setViewMode(ViewMode mode)
{
	if (d->viewMode == mode)
		return;

	d->config->group(u"Settings"_s).writeEntry("ViewMode", (int)mode);
	d->viewMode = mode;
	Q_EMIT viewModeChanged();
}

void SApp::openRightClickMenuForPlace(const QModelIndex& idx)
{
	QPointer<QMenu> menu(new QMenu);
	auto url = filePlacesModel->url(idx);

	menu->addAction(QIcon::fromTheme(u"tab-new"_s), i18n("Open In New Tab"), [=] {
		auto window = QGuiApplication::focusWindow();
		auto swindow = sApp->swindowForWindow(window);

		swindow->newDocumentAtUrl(url);
	});
	menu->addAction(QIcon::fromTheme(u"window-new"_s), i18n("Open In New Window"), [=] {
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
