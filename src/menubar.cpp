#include <KActionCollection>
#include <KLocalizedString>

#include <QDebug>
#include <QMenuBar>
#include <QMenu>
#include <QApplication>
#include <KIO/FileUndoManager>
#include <KFileItemActions>
#include <KFileItemListProperties>

#include "app.h"
#include "menubar.h"
#include "window.h"
#include "document.h"


// SHORTCUT CODE
#include <QtGui/private/qguiapplication_p.h>

static bool delfenojShortcutContextMatcher(QObject*, Qt::ShortcutContext context)
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
	pApp->shortcutMap.addShortcut(action, action->shortcut(), action->shortcutContext(), delfenojShortcutContextMatcher);
}

//

struct SMenuBar::Private
{
	QScopedPointer<QMenuBar> menuBar;
	QScopedPointer<KActionCollection> ac;
	QScopedPointer<KFileItemActions> fileItemActions;
};

SMenuBar::SMenuBar(QObject* parent) : QObject(parent), d(new Private)
{
	d->menuBar.reset(new QMenuBar(nullptr));
	d->ac.reset(new KActionCollection(d->menuBar.get()));
	d->fileItemActions.reset(new KFileItemActions);

	d->ac->add<QAction>(
		"about", this, &SMenuBar::about);

	d->ac->add<QAction>(
		"preferences", this, &SMenuBar::preferences);

	d->ac->add<QAction>(
		"empty_trash", this, &SMenuBar::emptyTrash);

	d->ac->add<QAction>(
		"new_window", this, &SMenuBar::newWindow);
	d->ac->add<QAction>(
		"new_tab", this, &SMenuBar::newTab);
	d->ac->add<QAction>(
		"new_folder", this, &SMenuBar::newFolder);


	d->ac->add<QAction>(
		"open", this, &SMenuBar::open);
	d->ac->add<QAction>(
		"close_window", this, &SMenuBar::closeWindow);

	d->ac->add<QAction>(
		"duplicate", this, &SMenuBar::duplicate);
	d->ac->add<QAction>(
		"make_alias", this, &SMenuBar::makeAlias);

	d->ac->add<QAction>(
		"move_to_trash", this, &SMenuBar::moveToTrash);

	d->ac->add<QAction>(
		"undo", this, &SMenuBar::undo);
	d->ac->add<QAction>(
		"redo", this, &SMenuBar::redo);

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

	Menu(i18n("Delfenoj"))
		Action("about", i18n("About Delfenoj"), )
			Separator
				Action("preferences", i18n("Preferences"), QKeySequence::Preferences)
					Separator
						Action("empty_trash", i18n("Empty Trash"), "Ctrl+Shift+Del")
							EndMenu

							Menu(i18n("File"))
								Action("new_window", i18n("New Window"), QKeySequence::New)
									Action("new_tab", i18n("New Tab"), QKeySequence::AddTab)
										Action("new_folder", i18n("New Folder..."), )
											Action("open", i18n("Open"), QKeySequence::Open)
												Action("close_window", i18n("Close Window"), QKeySequence::Close)
													Separator
														Action("duplicate", i18n("Duplicate"), "Ctrl+D")
															Action("make_alias", i18n("Make Alias"), "Ctrl+L")
																Separator
																	Action("move_to_trash", i18n("Move To Trash"), "Ctrl+Del")
																		EndMenu

																		Menu(i18n("Edit"))
																			Action("undo", i18n("Undo"), QKeySequence::Undo)
																				Action("redo", i18n("Redo"), QKeySequence::Redo)
																					Separator
																						Action("cut", i18n("Cut"), QKeySequence::Cut)
																							Action("copy", i18n("Copy"), QKeySequence::Copy)
																								Action("paste", i18n("Paste"), QKeySequence::Paste)
																									Action("select_all", i18n("Select All"), QKeySequence::SelectAll)
																										EndMenu

																										Menu(i18n("View"))
																											Action("view_as_icons", i18n("as Icons"), )
																												Action("view_as_list", i18n("as List"), )
																													Separator
																														Action("toggle_pathbar", i18n("Show Path Bar"), )
																															Action("toggle_statusbar", i18n("Show Status Bar"), )
																																Action("toggle_sidebar", i18n("Show Sidebar"), )
																																	Separator
																																		Action("toggle_toolbar", i18n("Show Toolbar"), )
																																			Action("customise_toolbar", i18n("Customise Toolbar..."), )
																																				EndMenu

																																				Menu(i18n("Go"))
																																					Action("back", i18n("Back"), QKeySequence::Back)
																																						Action("forward", i18n("Forward"), QKeySequence::Forward)
																																							Action("up", i18n("Containing Folder"), "Alt+Up")
																																								EndMenu

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
}

#define ActionForWindow \
	auto window = QGuiApplication::focusWindow(); \
	auto swindow = SApp::instance->swindowForWindow(window); \
	if (!swindow) return;

SMenuBar::~SMenuBar()
{
}

void SMenuBar::about()
{
	auto win = qobject_cast<QQuickWindow*>(SApp::instance->aboutComponent->create());
	win->show();
	// TODO: delete these windows
}

void SMenuBar::preferences()
{
	qFatal("Not implemented");
}

void SMenuBar::emptyTrash()
{
	qFatal("Not implemented");
}

void SMenuBar::newWindow()
{
	SApp::instance->newWindow();
}

void SMenuBar::newTab()
{
	ActionForWindow

		swindow->newDocument();
}

void SMenuBar::newFolder()
{
	qFatal("Not implemented");
}

void SMenuBar::open()
{
	ActionForWindow

		swindow->activeDocument()
			->openSelectedFiles();
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

void SMenuBar::redo()
{
	qFatal("Not implemented");
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
	qFatal("not implemented");
}

void SMenuBar::viewAsIcons()
{
	qFatal("Not implemented");
}

void SMenuBar::viewAsList()
{
	qFatal("Not implemented");
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
	qFatal("Not implemented");
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
