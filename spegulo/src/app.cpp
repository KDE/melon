#include <QQmlEngine>
#include <QQmlComponent>
#include <QFile>
#include <QQmlComponent>
#include <QMimeDatabase>
#include <QQuickWindow>
#include <KIO/FileJob>

#include "app.h"
#include "document.h"

struct SPApp::Private
{
	QQmlEngine *engine = nullptr;
	QMimeDatabase mimeDatabase;
	QList<SPDocument *> states;
	SPDocument *state(const QUrl &url)
	{
		for (auto *state : states) {
			if (state->url() == url)
				return state;
		}
		qFatal("asked to find state of %s, but failed to find one", qPrintable(url.toDisplayString()));
	}
	Components components;
	QScopedPointer<KSyntaxHighlighting::Repository> repository;
};

static SPApp *s_instance;

SPApp::SPApp(QObject *parent)
	: QObject(parent), d(new Private)
{
	s_instance = this;

	d->engine = new QQmlEngine(this);
	d->components.window = new QQmlComponent(d->engine);
	d->repository.reset(new KSyntaxHighlighting::Repository());

	auto make = [=](const QUrl& url, const QString& file, QQmlComponent*& component) {
		component = new QQmlComponent(d->engine);

		QFile fi(file);
		fi.open(QFile::ReadOnly);

		const auto data = fi.readAll();
		component->setData(data, url);
		if (component->isError()) {
			qWarning().noquote().nospace() << component->errorString();
		}
	};

	make(QUrl("qrc:/Window.qml"), ":/Window.qml", d->components.window);
	make(QUrl("qrc:/viewers/Plaintext.qml"), ":/viewers/Plaintext.qml", d->components.plaintextViewer);
	make(QUrl("qrc:/viewers/Unsupported.qml"), ":/viewers/Unsupported.qml", d->components.unsupportedViewer);
};

SPApp::~SPApp()
{
}

SPApp *SPApp::instance()
{
	return s_instance;
}

const Components &SPApp::components() const
{
	return d->components;
}

void SPApp::ShowFile(const QString &uri, const QString &parentWindowHandle)
{
	const auto url = QUrl(uri);

	const auto any = std::any_of(d->states.constBegin(), d->states.constEnd(), [=](SPDocument *it) {
		return it->url() == url;
	});
	if (any) {
		return;
	}

	auto doc = new SPDocument(url, this);
	d->states << doc;
	connect(doc, &SPDocument::closed, this, [=] { d->states.removeAll(doc); });

	auto job = KIO::open(QUrl(uri), QIODevice::ReadOnly);
	connect(job, &KIO::FileJob::result, this, [this, job] { fileResult(job); });
	connect(job, &KIO::FileJob::open, this, [this, job] { fileOpen(job); });
	connect(job, &KIO::FileJob::data, this, [this, job](KIO::Job *, const QByteArray &data) { fileData(job, data); });
	connect(job, &KIO::FileJob::mimeTypeFound, this, [this, job](KIO::Job *, const QString &data) { fileMimeTypeFound(job, data); });
}

void SPApp::fileResult(KIO::FileJob *job)
{
	if (job->error() != KJob::NoError) {
		d->state(job->url())->failed(job->errorString());
	}

	job->deleteLater();
}

void SPApp::fileOpen(KIO::FileJob *job)
{
	job->read(job->size());

	d->state(job->url())->opened();
}

void SPApp::fileData(KIO::FileJob *job, const QByteArray &data)
{
	Q_ASSERT(data.length() == job->size());

	d->state(job->url())->dataLoaded(data);
}

KSyntaxHighlighting::Repository* SPApp::syntaxHighlightingRepository() const
{
	return d->repository.get();
}

void SPApp::fileMimeTypeFound(KIO::FileJob *job, const QString &mimeType)
{
	d->state(job->url())->mimeTypeFound(d->mimeDatabase.mimeTypeForName(mimeType));
}
