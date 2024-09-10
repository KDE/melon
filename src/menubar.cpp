// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <KActionCollection>
#include <KLocalizedString>

#include <QDebug>
#include <QMenuBar>
#include <QMenu>
#include <QApplication>
#include <KIO/FileUndoManager>
#include <KIO/JobUiDelegate>
#include <KIO/JobUiDelegateFactory>
#include <KIO/EmptyTrashJob>
#include <KFileItemActions>
#include <KFileItemListProperties>
#include <KNewFileMenu>
#include <KBookmarkMenu>
#include <KBookmarkManager>
#include <KBookmarkAction>
#include <KBookmarkActionMenu>
#include <QMainWindow>
#include <QMessageBox>

#include "app.h"
#include "menubar.h"
#include "kio/jobuidelegateextension.h"
#include "kio/jobuidelegatefactory.h"
#include "kjobuidelegate.h"
#include "window.h"
#include "document.h"

using namespace Qt::StringLiterals;

// SHORTCUT CODE
#include <QtGui/private/qguiapplication_p.h>
#include <QtQuick/private/qquickevents_p_p.h>

static bool melonShortcutContextMatcher(QObject*, Qt::ShortcutContext context)
{
	switch (context) {
	case Qt::ApplicationShortcut:
		return true;
	default:
		return false;
	}
}

static void registerShortcut(QAction* action)
{
	if (action->shortcut().isEmpty())
		return;

	auto* pApp = QGuiApplicationPrivate::instance();
	pApp->shortcutMap.addShortcut(action, action->shortcut(), action->shortcutContext(), melonShortcutContextMatcher);
}

//

#define ActionForWindow \
	auto window = QGuiApplication::focusWindow(); \
	auto swindow = sApp->swindowForWindow(window); \
	if (!swindow) return;

struct SMenuBar::Private
{
	QScopedPointer<QMenuBar> menuBar;
	QScopedPointer<KActionCollection> ac;
	QScopedPointer<KFileItemActions> fileItemActions;
	QScopedPointer<KNewFileMenu> newFileMenu;
	QScopedPointer<QMenu> dummyMenu;
	QScopedPointer<KBookmarkMenu> bookmarksMenuManager;

	QMenu* melonMenu = nullptr;
	QMenu* fileMenu = nullptr;
	QMenu* editMenu = nullptr;
	QMenu* viewMenu = nullptr;
	QMenu* goMenu = nullptr;
	QAction* goAfterSeparator = nullptr;
	QList<QAction*> goActions;
};

QAction *getActionOrElse(QAction *action, const char *name, const QString &fallbackName, const QString &fallbackText)
{
	if (action != nullptr) {
		return action;
	}
	const auto act = new QAction(fallbackName);
	act->setObjectName(name);
	QObject::connect(act, &QAction::triggered, act, [fallbackText] {
		QMessageBox box;
		box.setText(fallbackText);
		box.exec();
	});
	return act;
}

SMenuBar::SMenuBar(QObject* parent) : QObject(parent), d(new Private)
{
	d->menuBar.reset(new QMenuBar(nullptr));
	d->ac.reset(new KActionCollection(d->menuBar.get()));
	d->fileItemActions.reset(new KFileItemActions);
	d->newFileMenu.reset(new KNewFileMenu(this));
	d->ac->addAction(u"new_file"_s, d->newFileMenu.get());
	d->dummyMenu.reset(new QMenu());
	d->bookmarksMenuManager.reset(new KBookmarkMenu(sApp->bookmarkManager(), this, d->dummyMenu.get()));

	const auto bookmarksAddAction = getActionOrElse(d->bookmarksMenuManager->addBookmarkAction(), "add_bookmark", u"(ERROR) Add Bookmark"_s, u"KBookmarkMenu didn't return the add bookmark action. Sorry."_s);
	d->ac->addAction(bookmarksAddAction->objectName(), bookmarksAddAction);

	const auto bookmarksEditAction = getActionOrElse(d->bookmarksMenuManager->editBookmarksAction(), "edit_bookmark", u"(ERROR) Edit Bookmarks"_s, u"KBookmarkMenu didn't return the edit bookmark action. Sorry."_s);
	d->ac->addAction(bookmarksEditAction->objectName(), bookmarksEditAction);

	auto bookmarksThunk = [this] {
		auto actionForBookmark = [=](const KBookmark& bm) -> QAction* {
			if (bm.isGroup()) {
				KActionMenu* actionMenu = new KBookmarkActionMenu(bm, this);
				d->goActions << actionMenu;
				new KBookmarkMenu(sApp->bookmarkManager(), this, actionMenu->menu(), bm.address());
				return actionMenu;
			} else if (bm.isSeparator()) {
				QAction* sa = new QAction(this);
				sa->setSeparator(true);
				d->goActions << sa;
				return sa;
			} else {
				QAction* action = new KBookmarkAction(bm, this, this);
				d->goActions << action;
				return action;
			}
		};

		for (auto* act : d->goActions) {
			d->goMenu->removeAction(act);
			act->deleteLater();
		}
		d->goActions.clear();

		int i = 0;
		const auto bookmark = sApp->bookmarkManager()->root();
		for (auto bm = bookmark.first(); !bm.isNull(); bm = bookmark.next(bm)) {
			i++;

			auto act = actionForBookmark(bm);
			if (i == 10) {
				act->setShortcut(QStringLiteral("Ctrl+Shift+0"));
			} else if (i < 10) {
				act->setShortcut(QStringLiteral("Ctrl+Shift+%1").arg(i));
			}
			if (!act->shortcut().isEmpty()) {
				act->setShortcutContext(Qt::ApplicationShortcut);
				registerShortcut(act);
			}
			d->goMenu->insertAction(d->goAfterSeparator, act);
		}
	};
	connect(sApp->bookmarkManager(), &KBookmarkManager::changed, this, bookmarksThunk);

	d->ac->add<QAction>(
		u"about"_s, this, &SMenuBar::about);

	d->ac->add<QAction>(
		u"preferences"_s, this, &SMenuBar::preferences);

	d->ac->add<QAction>(
		u"empty_trash"_s, this, &SMenuBar::emptyTrash);

	d->ac->add<QAction>(
		u"quit"_s, qApp, &QGuiApplication::quit);

	d->ac->add<QAction>(
		u"new_window"_s, this, &SMenuBar::newWindow);
	d->ac->add<QAction>(
		u"new_tab"_s, this, &SMenuBar::newTab);

	d->ac->add<QAction>(
		u"open"_s, this, &SMenuBar::open);
	d->ac->add<QAction>(
		u"close_window"_s, this, &SMenuBar::closeWindow);

	d->ac->add<QAction>(
		u"get_info"_s, this, &SMenuBar::getInfo);
	d->ac->add<QAction>(
		u"duplicate"_s, this, &SMenuBar::duplicate);
	d->ac->add<QAction>(
		u"make_alias"_s, this, &SMenuBar::makeAlias);

	d->ac->add<QAction>(
		u"move_to_trash"_s, this, &SMenuBar::moveToTrash);

	d->ac->add<QAction>(
		u"undo"_s, this, &SMenuBar::undo);

	d->ac->add<QAction>(
		u"cut"_s, this, &SMenuBar::cut);
	d->ac->add<QAction>(
		u"copy"_s, this, &SMenuBar::copy);
	d->ac->add<QAction>(
		u"paste"_s, this, &SMenuBar::paste);
	d->ac->add<QAction>(
		u"select_all"_s, this, &SMenuBar::selectAll);

	d->ac->add<QAction>(
		u"view_as_icons"_s, this, &SMenuBar::viewAsIcons);
	d->ac->add<QAction>(
		u"view_as_list"_s, this, &SMenuBar::viewAsList);
	d->ac->add<QAction>(
		u"view_as_columns"_s, this, &SMenuBar::viewAsColumns);

	d->ac->add<QAction>(
		u"toggle_pathbar"_s, this, &SMenuBar::togglePathBar);
	d->ac->add<QAction>(
		u"toggle_statusbar"_s, this, &SMenuBar::toggleStatusBar);
	d->ac->add<QAction>(
		u"toggle_sidebar"_s, this, &SMenuBar::toggleSidebar);

	d->ac->add<QAction>(
		u"toggle_toolbar"_s, this, &SMenuBar::toggleToolbar);
	d->ac->add<QAction>(
		u"customise_toolbar"_s, this, &SMenuBar::customiseToolbar);

	d->ac->add<QAction>(
		u"back"_s, this, &SMenuBar::back);
	d->ac->add<QAction>(
		u"forward"_s, this, &SMenuBar::forward);
	d->ac->add<QAction>(
		u"up"_s, this, &SMenuBar::up);

#define Menu(name) \
	{ \
		auto menu = d->menuBar->addMenu(name);
#define Action(code, name, short) \
	{ \
		auto action = d->ac->action(code); \
		action->setText(name); \
		action->setShortcut(QKeySequence(short)); \
		menu->addAction(action); \
		action->setShortcutContext(Qt::ApplicationShortcut); \
		registerShortcut(action); \
	}
#define Separator menu->addSeparator();
#define EndMenu }

	// clang-format off
	Menu(i18n("Melon"))
		d->melonMenu = menu;

		Action(u"about"_s, i18n("About Melon"), )
		Separator
		Action(u"preferences"_s, i18n("Preferences"), QKeySequence::Preferences)
		Separator
		Action(u"empty_trash"_s, i18n("Empty Trash"), u"Ctrl+Shift+Del"_s)
		Separator
		Action(u"quit"_s, i18n("Quit Melon"), QKeySequence::Quit)
	EndMenu

	Menu(i18n("File"))
		d->fileMenu = menu;

		connect(menu, &QMenu::aboutToShow, d->newFileMenu.get(), [this] {
			ActionForWindow

			d->newFileMenu->setWorkingDirectory(swindow->activeDocument()->actualViewingURL());
			d->newFileMenu->checkUpToDate();
		});

		Action(u"new_window"_s, i18n("New Window"), QKeySequence::New)
		Action(u"new_tab"_s, i18n("New Tab"), QKeySequence::AddTab)
		Action(u"new_file"_s, i18n("New File"),)
		Action(u"open"_s, i18n("Open"), QKeySequence::Open)
		Action(u"close_window"_s, i18n("Close Window"), QKeySequence::Close)
		Separator
		Action(u"get_info"_s, i18n("Get Info"), u"Ctrl+I"_s)
		Action(u"duplicate"_s, i18n("Duplicate"), u"Ctrl+D"_s)
		Action(u"make_alias"_s, i18n("Make Alias"), u"Ctrl+L"_s)
		Separator
		Action(u"move_to_trash"_s, i18n("Move To Trash"), u"Ctrl+Del"_s)
	EndMenu

	Menu(i18n("Edit"))
		d->editMenu = menu;

		Action(u"undo"_s, i18n("Undo"), QKeySequence::Undo)
		Separator
		Action(u"cut"_s, i18n("Cut"), QKeySequence::Cut)
		Action(u"copy"_s, i18n("Copy"), QKeySequence::Copy)
		Action(u"paste"_s, i18n("Paste"), QKeySequence::Paste)
		Action(u"select_all"_s, i18n("Select All"), QKeySequence::SelectAll)
	EndMenu

	Menu(i18n("View"))
		d->viewMenu = menu;

		Action(u"view_as_icons"_s, i18n("as Icons"), )
		Action(u"view_as_list"_s, i18n("as List"), )
		Action(u"view_as_columns"_s, i18n("as Columns"), )
		Separator
		Action(u"toggle_pathbar"_s, i18n("Show Path Bar"), )
		Action(u"toggle_statusbar"_s, i18n("Show Status Bar"), )
		Action(u"toggle_sidebar"_s, i18n("Show Sidebar"), )
		Separator
		Action(u"toggle_toolbar"_s, i18n("Show Toolbar"), )
		Action(u"customise_toolbar"_s, i18n("Customise Toolbar..."), )
	EndMenu

	Menu(i18n("Go"))
		d->goMenu = menu;

		Action(u"back"_s, i18n("Back"), QKeySequence::Back)
		Action(u"forward"_s, i18n("Forward"), QKeySequence::Forward)
		Action(u"up"_s, i18n("Containing Folder"), u"Alt+Up"_s)
		Separator

		d->bookmarksMenuManager->ensureUpToDate();

		d->goAfterSeparator = Separator;

		Action(bookmarksAddAction->objectName(), i18n("Bookmark Current Folder"), u"Ctrl+B"_s)
		Action(bookmarksEditAction->objectName(), i18n("Edit Bookmarks..."),)

	EndMenu
		// clang-format on

		bookmarksThunk();

#define GA(name) d->ac->action(name)
	{
		auto evaluate = [act = GA(u"toggle_sidebar"_s)] {
			if (sApp->showSidebar())
				act->setText(i18n("Hide Sidebar"));
			else
				act->setText(i18n("Show Sidebar"));
		};
		evaluate();
		connect(sApp, &SApp::showSidebarChanged, this, evaluate);
	}
	{
		auto evaluate = [act = GA(u"toggle_pathbar"_s)] {
			if (sApp->showPathBar())
				act->setText(i18n("Hide Path Bar"));
			else
				act->setText(i18n("Show Path Bar"));
		};
		evaluate();
		connect(sApp, &SApp::showPathBarChanged, this, evaluate);
	}
	{
		auto evaluate = [act = GA(u"toggle_statusbar"_s)] {
			if (sApp->showStatusBar())
				act->setText(i18n("Hide Status Bar"));
			else
				act->setText(i18n("Show Status Bar"));
		};
		evaluate();
		connect(sApp, &SApp::showStatusBarChanged, this, evaluate);
	}
	{
		auto evaluate = [act = GA(u"toggle_toolbar"_s)] {
			if (sApp->showToolbar())
				act->setText(i18n("Hide Toolbar"));
			else
				act->setText(i18n("Show Toolbar"));
		};
		evaluate();
		connect(sApp, &SApp::showToolbarChanged, this, evaluate);
	}
	{
		auto evaluate = [act = GA(u"customise_toolbar"_s)] {
			if (sApp->editMode())
				act->setText(i18n("Finish Customising Toolbar"));
			else
				act->setText(i18n("Customise Toolbar..."));
		};
		evaluate();
		connect(sApp, &SApp::editModeChanged, this, evaluate);
	}
}

SMenuBar::~SMenuBar()
{
}

QMenuBar *SMenuBar::createMenuBarFor(QWidget *parent)
{
	auto mbar = new QMenuBar(parent);
	mbar->addMenu(d->melonMenu);
	mbar->addMenu(d->fileMenu);
	mbar->addMenu(d->editMenu);
	mbar->addMenu(d->viewMenu);
	mbar->addMenu(d->goMenu);
	return mbar;
}

QString SMenuBar::currentTitle() const
{
    auto window = QGuiApplication::focusWindow();
    auto swindow = sApp->swindowForWindow(window);
    if (!swindow)
		return {};

	return swindow->activeDocument()->fancyNameFor(swindow->activeDocument()->navigator()->currentLocationUrl());
}

QUrl SMenuBar::currentUrl() const
{
    auto window = QGuiApplication::focusWindow();
    auto swindow = sApp->swindowForWindow(window);
    if (!swindow)
		return {};

	return swindow->activeDocument()->navigator()->currentLocationUrl();
}

bool SMenuBar::enableOption(BookmarkOption option) const
{
	switch (option) {
	case BookmarkOption::ShowAddBookmark:
	case BookmarkOption::ShowEditBookmark:
		return true;
	default:
		return false;
	}
}

void SMenuBar::openBookmark(const KBookmark& bookmark, Qt::MouseButtons, Qt::KeyboardModifiers)
{
	ActionForWindow

	swindow->activeDocument()->openUrl(bookmark.url());
}

void SMenuBar::about()
{
	auto win = qobject_cast<QQuickWindow*>(sApp->aboutComponent->create());
	win->show();
	connect(win, &QQuickWindow::closing, this, [win]() {
		win->deleteLater();
	});
}

void SMenuBar::preferences()
{
	QMessageBox box;
	box.setText(u"Not implemented"_s);
	box.exec();
}

void SMenuBar::emptyTrash()
{
	auto delegate = KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr);
	auto extension = dynamic_cast<KIO::JobUiDelegateExtension*>(delegate);
	if (extension == nullptr) {
		qFatal("KIO didn't give us a job delegate that can ask for deletion!");
	}
	bool confirmed = extension->askDeleteConfirmation(QList<QUrl>(), KIO::JobUiDelegate::EmptyTrash, KIO::JobUiDelegate::ForceConfirmation);
	if (confirmed) {
		KIO::emptyTrash();
	}
	delegate->deleteLater();
}

void SMenuBar::newWindow()
{
	sApp->newWindow();
}

void SMenuBar::newTab()
{
	ActionForWindow

		swindow->newDocument();
}

void SMenuBar::open()
{
	ActionForWindow

		swindow->activeDocument()
			->openSelectedFiles();
}

void SMenuBar::getInfo()
{
	ActionForWindow

		swindow->activeDocument()
			->getInfoOnSelectedFiles();
}

void SMenuBar::closeWindow()
{
	ActionForWindow

		window->close();
}

void SMenuBar::duplicate()
{
	ActionForWindow

		swindow->activeDocument()
			->duplicateSelectedFiles();
}

void SMenuBar::makeAlias()
{
	ActionForWindow

		swindow->activeDocument()
			->aliasSelectedFiles();
}

void SMenuBar::moveToTrash()
{
	ActionForWindow

		swindow->activeDocument()
			->trashSelectedFiles();
}

void SMenuBar::undo()
{
	KIO::FileUndoManager::self()->undo();
}

void SMenuBar::cut()
{
	QMessageBox box;
	box.setText(u"Not implemented"_s);
	box.exec();
}

void SMenuBar::copy()
{
	ActionForWindow

		swindow->activeDocument()
			->copy();
}

void SMenuBar::paste()
{
	ActionForWindow

		swindow->activeDocument()
			->paste();
}

void SMenuBar::selectAll()
{
	ActionForWindow

		// TODO: column view
		auto sm = swindow->activeDocument()->selectionModelFor(swindow->activeDocument()->currentDirModel());
	auto index = sm->model()->index(0, 0);
	sm->select(index, QItemSelectionModel::Select | QItemSelectionModel::Columns);
}

void SMenuBar::viewAsIcons()
{
	sApp->setViewMode(SApp::Icons);
}

void SMenuBar::viewAsList()
{
	sApp->setViewMode(SApp::List);
}

void SMenuBar::viewAsColumns()
{
	sApp->setViewMode(SApp::Columns);
}

void SMenuBar::togglePathBar()
{
	sApp->setShowPathBar(!sApp->showPathBar());
}

void SMenuBar::toggleStatusBar()
{
	sApp->setShowStatusBar(!sApp->showStatusBar());
}

void SMenuBar::toggleSidebar()
{
	sApp->setShowSidebar(!sApp->showSidebar());
}

void SMenuBar::toggleToolbar()
{
	sApp->setShowToolbar(!sApp->showToolbar());
}

void SMenuBar::customiseToolbar()
{
	sApp->setEditMode(!sApp->editMode());
}

void SMenuBar::back()
{
	ActionForWindow

		swindow->activeDocument()
			->navigator()
			->goBack();
}

void SMenuBar::forward()
{
	ActionForWindow

		swindow->activeDocument()
			->navigator()
			->goForward();
}

void SMenuBar::up()
{
	ActionForWindow

		swindow->activeDocument()
			->navigator()
			->goUp();
}
