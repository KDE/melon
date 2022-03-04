#pragma once

#include <QQuickWindow>
#include <QQmlEngine>
#include <QQmlComponent>

class SWindow;

struct SApp
{
	static SApp* instance;

	QList<SWindow*> windows;
	QScopedPointer<QQmlEngine> engine;
	QScopedPointer<QQmlComponent> windowComponent;
	QScopedPointer<QQmlComponent> pageComponent;

	SApp();
	~SApp();
	void start();
	void newDocument();
};
