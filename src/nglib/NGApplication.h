#pragma once

#include <QApplication>
#include <QObject>

#include <KConfigGroup>

#define NGApplicationSuperclass QApplication

class NGSavable;

class NGApplication : public NGApplicationSuperclass
{
	Q_OBJECT

	struct Private;
	QScopedPointer<Private> d;

	void saveSavables(KConfigGroup& group);
	void saveSavablesConfig();
	void saveSavablesSession(QSessionManager& manager);

public:
	NGApplication(int& argc, char** argv);
	~NGApplication();

	void registerSavable(NGSavable* savable);
	void unregisterSavable(NGSavable* savable);
};

#ifdef qApp
#undef qApp
#define qApp qobject_cast<NGApplication*>(QCoreApplication::instance())
#endif
