#include <KActionCollection>
#include <KLocalizedString>

#include <QDebug>
#include <QMenuBar>
#include <QMenu>
#include <QApplication>
#include <KIO/FileUndoManager>
#include <KIO/JobUiDelegate>
#include <KIO/EmptyTrashJob>
#include <KFileItemActions>
#include <KFileItemListProperties>
#include <KNewFileMenu>
#include <KBookmarkMenu>
#include <KBookmarkManager>
#include <KBookmarkAction>
#include <KBookmarkActionMenu>

#include "app.h"
#include "menubar.h"
#include "window.h"
#include "document.h"


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

	QMenu* goMenu = nullptr;
	QAction* goAfterSeparator = nullptr;
	QList<QAction*> goActions;
};

SMenuBar::SMenuBar(QObject* parent) : QObject(parent), d(new Private)
{
	d->menuBar.reset(new QMenuBar(nullptr));
	d->ac.reset(new KActionCollection(d->menuBar.get()));
	d->fileItemActions.reset(new KFileItemActions);
	d->newFileMenu.reset(new KNewFileMenu(d->ac.get(), "new_file", this));
	d->dummyMenu.reset(new QMenu());
	d->bookmarksMenuManager.reset(new KBookmarkMenu(sApp->bookmarkManager(), this, d->dummyMenu.get()));

	const auto bookmarksAddAction = d->bookmarksMenuManager->addBookmarkAction();
	d->ac->addAction(bookmarksAddAction->objectName(), bookmarksAddAction);

	const auto bookmarksEditAction = d->bookmarksMenuManager->editBookmarksAction();
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
				act->setShortcut(QString("Ctrl+Shift+0"));
			} else if (i < 10) {
				act->setShortcut(QString("Ctrl+Shift+%1").arg(i));
			}
			if (!act->shortcut().isEmpty()) {
				act->setShortcutContext(Qt::ApplicationShortcut);
				registerShortcut(act);
			}
			d->goMenu->insertAction(d->goAfterSeparator, act);
		}
	};
	connect(sApp->bookmarkManager(), &KBookmarkManager::bookmarksChanged, this, bookmarksThunk);

	d->ac->add<QAction>(
		"about", this, &SMenuBar::about);

	d->ac->add<QAction>(
		"preferences", this, &SMenuBar::preferences);

	d->ac->add<QAction>(
		"empty_trash", this, &SMenuBar::emptyTrash);

	d->ac->add<QAction>(
		"quit", qApp, &QGuiApplication::quit);

	d->ac->add<QAction>(
		"new_window", this, &SMenuBar::newWindow);
	d->ac->add<QAction>(
		"new_tab", this, &SMenuBar::newTab);

	d->ac->add<QAction>(
		"open", this, &SMenuBar::open);
	d->ac->add<QAction>(
		"close_window", this, &SMenuBar::closeWindow);

	d->ac->add<QAction>(
		"get_info", this, &SMenuBar::getInfo);
	d->ac->add<QAction>(
		"duplicate", this, &SMenuBar::duplicate);
	d->ac->add<QAction>(
		"make_alias", this, &SMenuBar::makeAlias);

	d->ac->add<QAction>(
		"move_to_trash", this, &SMenuBar::moveToTrash);

	d->ac->add<QAction>(
		"undo", this, &SMenuBar::undo);

	d->ac->add<QAction>(
		"cut", this, &SMenuBar::cut);
	d->ac->add<QAction>(
		"copy", this, &SMenuBar::copy);
	d->ac->add<QAction>(
		"paste", this, &SMenuBar::paste);
	d->ac->add<QAction>(
		"select_all", this, &SMenuBar::selectAll);

	d->ac->add<QAction>(
		"view_as_icons", this, &SMenuBar::viewAsIcons);
	d->ac->add<QAction>(
		"view_as_list", this, &SMenuBar::viewAsList);
	d->ac->add<QAction>(
		"view_as_columns", this, &SMenuBar::viewAsColumns);

	d->ac->add<QAction>(
		"toggle_pathbar", this, &SMenuBar::togglePathBar);
	d->ac->add<QAction>(
		"toggle_statusbar", this, &SMenuBar::toggleStatusBar);
	d->ac->add<QAction>(
		"toggle_sidebar", this, &SMenuBar::toggleSidebar);

	d->ac->add<QAction>(
		"toggle_toolbar", this, &SMenuBar::toggleToolbar);
	d->ac->add<QAction>(
		"customise_toolbar", this, &SMenuBar::customiseToolbar);

	d->ac->add<QAction>(
		"back", this, &SMenuBar::back);
	d->ac->add<QAction>(
		"forward", this, &SMenuBar::forward);
	d->ac->add<QAction>(
		"up", this, &SMenuBar::up);

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
		Action("about", i18n("About Melon"), )
		Separator
		Action("preferences", i18n("Preferences"), QKeySequence::Preferences)
		Separator
		Action("empty_trash", i18n("Empty Trash"), "Ctrl+Shift+Del")
		Separator
		Action("quit", i18n("Quit Melon"), QKeySequence::Quit)
	EndMenu

	Menu(i18n("File"))
		connect(menu, &QMenu::aboutToShow, d->newFileMenu.get(), [this] {
			ActionForWindow

			d->newFileMenu->setPopupFiles(QList<QUrl> {swindow->activeDocument()->actualViewingURL()});
			d->newFileMenu->checkUpToDate();
		});

		Action("new_window", i18n("New Window"), QKeySequence::New)
		Action("new_tab", i18n("New Tab"), QKeySequence::AddTab)
		Action("new_file", i18n("New File"),)
		Action("open", i18n("Open"), QKeySequence::Open)
		Action("close_window", i18n("Close Window"), QKeySequence::Close)
		Separator
		Action("get_info", i18n("Get Info"), "Ctrl+I")
		Action("duplicate", i18n("Duplicate"), "Ctrl+D")
		Action("make_alias", i18n("Make Alias"), "Ctrl+L")
		Separator
		Action("move_to_trash", i18n("Move To Trash"), "Ctrl+Del")
	EndMenu

	Menu(i18n("Edit"))
		Action("undo", i18n("Undo"), QKeySequence::Undo)
		Separator
		Action("cut", i18n("Cut"), QKeySequence::Cut)
		Action("copy", i18n("Copy"), QKeySequence::Copy)
		Action("paste", i18n("Paste"), QKeySequence::Paste)
		Action("select_all", i18n("Select All"), QKeySequence::SelectAll)
	EndMenu

	Menu(i18n("View"))
		Action("view_as_icons", i18n("as Icons"), )
		Action("view_as_list", i18n("as List"), )
		Action("view_as_columns", i18n("as Columns"), )
		Separator
		Action("toggle_pathbar", i18n("Show Path Bar"), )
		Action("toggle_statusbar", i18n("Show Status Bar"), )
		Action("toggle_sidebar", i18n("Show Sidebar"), )
		Separator
		Action("toggle_toolbar", i18n("Show Toolbar"), )
		Action("customise_toolbar", i18n("Customise Toolbar..."), )
	EndMenu

	Menu(i18n("Go"))
		d->goMenu = menu;

		Action("back", i18n("Back"), QKeySequence::Back)
		Action("forward", i18n("Forward"), QKeySequence::Forward)
		Action("up", i18n("Containing Folder"), "Alt+Up")
		Separator

		d->bookmarksMenuManager->ensureUpToDate();

		d->goAfterSeparator = Separator;

		Action(bookmarksAddAction->objectName(), i18n("Bookmark Current Folder"), "Ctrl+B")
		Action(bookmarksEditAction->objectName(), i18n("Edit Bookmarks..."),)

	EndMenu
		// clang-format on

		bookmarksThunk();

#define GA(name) d->ac->action(name)
	{
		auto evaluate = [act = GA("toggle_sidebar")] {
			if (sApp->showSidebar())
				act->setText("Hide Sidebar");
			else
				act->setText("Show Sidebar");
		};
		evaluate();
		connect(sApp, &SApp::showSidebarChanged, this, evaluate);
	}
	{
		auto evaluate = [act = GA("toggle_pathbar")] {
			if (sApp->showPathBar())
				act->setText("Hide Path Bar");
			else
				act->setText("Show Path Bar");
		};
		evaluate();
		connect(sApp, &SApp::showPathBarChanged, this, evaluate);
	}
	{
		auto evaluate = [act = GA("toggle_statusbar")] {
			if (sApp->showStatusBar())
				act->setText("Hide Status Bar");
			else
				act->setText("Show Status Bar");
		};
		evaluate();
		connect(sApp, &SApp::showStatusBarChanged, this, evaluate);
	}
	{
		auto evaluate = [act = GA("toggle_toolbar")] {
			if (sApp->showToolbar())
				act->setText("Hide Toolbar");
			else
				act->setText("Show Toolbar");
		};
		evaluate();
		connect(sApp, &SApp::showToolbarChanged, this, evaluate);
	}
	{
		auto evaluate = [act = GA("customise_toolbar")] {
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
	qFatal("Not implemented");
}

void SMenuBar::emptyTrash()
{
	KIO::JobUiDelegate delegate;
	bool confirmed = delegate.askDeleteConfirmation(QList<QUrl>(), KIO::JobUiDelegate::EmptyTrash, KIO::JobUiDelegate::ForceConfirmation);
	if (confirmed) {
		KIO::Job* job = KIO::emptyTrash();
	}
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
	qFatal("Not implemented");
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
