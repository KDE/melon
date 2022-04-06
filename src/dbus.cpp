#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDir>
#include <QUrl>

#include <KWindowSystem>

#include "app.h"
#include "dbus.h"

SOrgFreedesktopFilemanager1::SOrgFreedesktopFilemanager1(QObject* parent) : QObject(parent)
{
	QDBusConnection::sessionBus().registerObject(
		"/org/freedesktop/FileManager1", this,
		QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAdaptors);

	auto sessionInterface = QDBusConnection::sessionBus().interface();
	if (sessionInterface) {
		sessionInterface->registerService(QStringLiteral("org.freedesktop.FileManager1")).error();
	}
}

SOrgFreedesktopFilemanager1::~SOrgFreedesktopFilemanager1()
{
}

auto toUriList(const QStringList& strl)
{
	const auto cwd = QDir::currentPath();
	QList<QUrl> urls;
	urls.reserve(strl.length());

	for (const QString& str : strl) {
		urls << QUrl::fromUserInput(str, cwd, QUrl::AssumeLocalFile);
	}

	return urls;
}

void SOrgFreedesktopFilemanager1::ShowFolders(const QStringList& uriList, const QString& startupID)
{
	auto urls = toUriList(uriList);
	KWindowSystem::setCurrentXdgActivationToken(startupID);
	for (const auto& url : urls)
		SApp::instance->ensureShown(url);
}

void SOrgFreedesktopFilemanager1::ShowItems(const QStringList& uriList, const QString& startupID)
{
	auto urls = toUriList(uriList);
	KWindowSystem::setCurrentXdgActivationToken(startupID);
	for (const auto& url : urls)
		SApp::instance->ensureShown(url);
}

void SOrgFreedesktopFilemanager1::ShowItemProperties(const QStringList& uriList, const QString& startupID)
{
	Q_UNUSED(uriList)
	Q_UNUSED(startupID)

	// we don't support this yet
	// auto urls = toUriList(uriList);
}
