#pragma once

#include <QQuickWindow>
#include <QQmlEngine>
#include <QQmlComponent>

struct SApp
{
	static SApp* instance;

	// std::vector<std::unique_ptr<BOpenDocument>> documents;
	QScopedPointer<QQmlEngine> engine;
	QScopedPointer<QQmlComponent> windowComponent;

	SApp();
	~SApp();
	void start();
	void newDocument();
};
