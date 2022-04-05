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

public:

	static SApp* instance;

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
};
