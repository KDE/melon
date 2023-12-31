#include <KWindowConfig>
#include <QUuid>
#include <QtQuick/private/qquickevents_p_p.h>

#include "window.h"
#include "document.h"
#include "app.h"

using namespace Qt::StringLiterals;

struct SWindow::Private
{
	QQuickWindow* displayedIn;
	QList<SDocument*> containing;
	SDocument* activeDocument = nullptr;
	QUuid uuid;
};

SWindow::SWindow(QQuickWindow* displayedIn, QObject* parent) : QObject(parent), d(new Private)
{
	d->displayedIn = displayedIn;
	newDocument();
	init();
}

SWindow::SWindow(const QUrl& in, QQuickWindow* displayedIn, QObject* parent) : QObject(parent), d(new Private)
{
	d->displayedIn = displayedIn;
	d->containing << new SDocument(in, this);
	Q_EMIT documentsChanged();
	init();
}

SWindow::SWindow(QUuid id, const KConfigGroup& config, QQuickWindow* displayedIn, QObject* parent) : QObject(parent), d(new Private)
{
	d->displayedIn = displayedIn;
	d->uuid = id;

	KWindowConfig::restoreWindowPosition(d->displayedIn, config);
	KWindowConfig::restoreWindowSize(d->displayedIn, config);

	for (const auto& groupName : config.groupList()) {
		if (!groupName.startsWith(u"document"_s)) {
			continue;
		}
		const auto group = config.group(groupName);

		d->containing << new SDocument(group, this);
	}

	Q_EMIT documentsChanged();
	init();
}

void SWindow::afterComponentComplete(const KConfigGroup& config)
{
	KWindowConfig::restoreWindowPosition(d->displayedIn, config);
	KWindowConfig::restoreWindowSize(d->displayedIn, config);
}

NGSavable::SaveInformation SWindow::save(KConfigGroup& config) const
{
	KWindowConfig::saveWindowPosition(d->displayedIn, config);
	KWindowConfig::saveWindowSize(d->displayedIn, config);

	for (const auto* document : d->containing) {
		auto subgroup = config.group(u"document-"_s + document->id().toString(QUuid::WithoutBraces));
		document->saveTo(subgroup);
	}

	return SaveInformation{u"SWindowRestorer*"_s, true};
}

QUuid SWindow::identifier() const
{
	return d->uuid;
}

void SWindow::init()
{
	if (d->uuid.isNull())
		d->uuid = QUuid::createUuid();

	qApp->registerSavable(this);

	auto win = d->displayedIn;
	connect(win, &QQuickWindow::closing, this, [this, win]() {
		win->deleteLater();
		Q_EMIT closing(this);
	});
}

SWindow::~SWindow()
{
	qApp->unregisterSavable(this);
}

QList<SDocument*> SWindow::documents() const
{
	return d->containing;
}

QQuickWindow* SWindow::displayedIn() const
{
	return d->displayedIn;
}

SDocument* SWindow::activeDocument() const
{
	return d->activeDocument;
}

void SWindow::setActiveDocument(SDocument* document)
{
	if (d->activeDocument == document)
		return;

	d->activeDocument = document;
	Q_EMIT activeDocumentChanged();
}

void SWindow::newDocument()
{
	d->containing << new SDocument(this);
	Q_EMIT documentsChanged();
}

void SWindow::newDocumentAtUrl(const QUrl& url)
{
	d->containing << new SDocument(url, this);
	Q_EMIT documentsChanged();
}

void SWindow::closeDocument(int idx)
{
	auto taken = d->containing.takeAt(idx);
	Q_EMIT documentsChanged();
	taken->deleteLater();
}

void SWindow::transferDocumentTo(SDocument* document, SWindow* window)
{
	Q_ASSERT(d->containing.contains(document));

	window->d->containing << document;
	Q_EMIT window->documentsChanged();
	d->containing.removeAll(document);
	if (d->containing.isEmpty()) {
		d->containing << new SDocument(this);
	}
	Q_EMIT documentsChanged();
}
