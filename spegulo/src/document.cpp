#include <QQuickWindow>
#include <QTextDocument>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QApplication>
#include <QPalette>
#include <QTextDocumentFragment>
#include <QTextCursor>
#include <QQuickTextDocument>

#include <KSyntaxHighlighting/syntaxhighlighter.h>
#include <KSyntaxHighlighting/definition.h>
#include <KSyntaxHighlighting/theme.h>
#include <KFileItemActions>

#include "document.h"
#include "app.h"
#include "service.h"

struct SPDocument::Private
{
	QUrl url;
	QQuickWindow* window = nullptr;
	State state = State::Loading;
	QMimeType mimeType;
	QByteArray data;
	QString failureString;
	QObject* viewer = nullptr;
	QList<SPService> services;
};

SPDocument::SPDocument(const QUrl& url, QObject* parent) : QObject(parent), d(new Private)
{
	d->url = url;
	d->window = qobject_cast<QQuickWindow*>(spApp->components().window->createWithInitialProperties(
		{{"document", QVariant::fromValue(this)}}));

	connect(d->window, &QQuickWindow::closing, this, &SPDocument::closed);
}

SPDocument::~SPDocument()
{
}

QUrl SPDocument::url() const
{
	return d->url;
}

QString SPDocument::title() const
{
	return d->url.fileName();
}

SPDocument::State SPDocument::state() const
{
	return d->state;
}

QString SPDocument::failureString() const
{
	return d->failureString;
}

void SPDocument::opened()
{
	Q_ASSERT(d->state == Loading || d->state == LoadingMimetypeKnown);
}

void SPDocument::mimeTypeFound(QMimeType mimeType)
{
	Q_ASSERT(d->state == Loading);
	d->state = LoadingMimetypeKnown;
	d->mimeType = mimeType;

	Q_EMIT stateChanged();

	const auto services = KFileItemActions::associatedApplications({mimeType.name()});
	for (auto& service : services) {
		d->services << SPService(service);
	}

	Q_EMIT defaultAppsChanged();

	instantiateComponent();
}

QVariantList SPDocument::defaultApps() const
{
	QVariantList apps;
	for (const auto& service : d->services)
		apps << QVariant::fromValue(service);

	return apps;
}

void SPDocument::dataLoaded(const QByteArray& data)
{
	Q_ASSERT(d->state == LoadingMimetypeKnown);
	d->state = Loaded;
	d->data = data;

	Q_EMIT stateChanged();

	insertData();
}

void SPDocument::failed(const QString& uiString)
{
	d->state = Failed;
	d->failureString = uiString;

	Q_EMIT stateChanged();
}

void SPDocument::instantiateComponent()
{
	QObject* viewer = nullptr;
	if (d->mimeType.inherits("text/plain")) {
		viewer = spApp->components().plaintextViewer->createWithInitialProperties({
			{"document", QVariant::fromValue(this)},
			{"text", ""},
		});
	} else {
		viewer = spApp->components().unsupportedViewer->createWithInitialProperties({
			{"document", QVariant::fromValue(this)},
		});
	}

	d->window->setProperty("innerItem", QVariant::fromValue(viewer));
	d->viewer = viewer;
}

void SPDocument::insertData()
{
	if (d->mimeType.inherits("text/plain")) {
		auto* repo = spApp->syntaxHighlightingRepository();
		auto def = repo->definitionForFileName(d->url.fileName());
		if (!def.isValid()) {
			def = repo->definitionForMimeType(d->mimeType.name());
		}
		auto doc = d->viewer->property("textDocument").value<QQuickTextDocument*>()->textDocument();
		if (def.isValid()) {
			doc->setDefaultFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
			auto theme = repo->themeForPalette(qApp->palette());
			auto high = new KSyntaxHighlighting::SyntaxHighlighter(doc);
			high->setDefinition(def);
			high->setTheme(theme);
			high->rehighlight();
		}
		d->viewer->setProperty("text", QString::fromLocal8Bit(d->data));
	}
}
