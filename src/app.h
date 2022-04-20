#pragma once

#include <QQuickWindow>
#include <QQmlEngine>
#include <QQmlComponent>
#include <KFilePlacesModel>

#include "menubar.h"

class SWindow;

struct SApp : public QObject
{
	Q_OBJECT

	struct Private;
	QScopedPointer<Private> d;


public:
	static SApp* instance();

	QList<SWindow*> windows;
	QScopedPointer<QQmlEngine> engine;
	QScopedPointer<QQmlComponent> windowComponent;
	QScopedPointer<QQmlComponent> pageComponent;
	QScopedPointer<QQmlComponent> aboutComponent;
	KFilePlacesModel* filePlacesModel;
	SMenuBar* sMenuBar;

	Q_PROPERTY(KFilePlacesModel* placesModel READ placesModel CONSTANT)
	KFilePlacesModel* placesModel() const;

	SApp();
	~SApp();
	void start();
	Q_INVOKABLE void newWindow();
	Q_INVOKABLE void newWindowAtUrl(const QUrl& url);
	Q_INVOKABLE void ensureShown(const QUrl& url);
	SWindow* swindowForWindow(QWindow* window);
	void windowClosing(SWindow* window);

	Q_INVOKABLE QString kaomoji(const QString& str);

	Q_PROPERTY(bool showSidebar READ showSidebar WRITE setShowSidebar NOTIFY showSidebarChanged)
	bool showSidebar() const;
	void setShowSidebar(bool show);
	Q_SIGNAL void showSidebarChanged();

	Q_PROPERTY(bool showStatusBar READ showStatusBar WRITE setShowStatusBar NOTIFY showStatusBarChanged)
	bool showStatusBar() const;
	void setShowStatusBar(bool show);
	Q_SIGNAL void showStatusBarChanged();

	Q_PROPERTY(bool showToolbar READ showToolbar WRITE setShowToolbar NOTIFY showToolbarChanged)
	bool showToolbar() const;
	void setShowToolbar(bool show);
	Q_SIGNAL void showToolbarChanged();

	Q_PROPERTY(bool showPathBar READ showPathBar WRITE setShowPathBar NOTIFY showPathBarChanged)
	bool showPathBar() const;
	void setShowPathBar(bool show);
	Q_SIGNAL void showPathBarChanged();

	Q_INVOKABLE void openRightClickMenuForPlace(const QModelIndex& idx);
};

#define sApp SApp::instance()
