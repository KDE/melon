#include "document.h"

struct SDocument::Private
{
	SWindow* window;
	KDirModel* dirModel;
	KCoreUrlNavigator* dirNavigator;
};

SDocument::SDocument(SWindow* parent) : QObject(parent), d(new Private)
{
	d->window = parent;
	d->dirModel = new KDirModel(this);
	d->dirNavigator = new KCoreUrlNavigator(QUrl::fromLocalFile("/etc"), this);
	connect(d->dirNavigator, &KCoreUrlNavigator::currentLocationUrlChanged, this, [this]() {
		d->dirModel->openUrl(d->dirNavigator->currentLocationUrl());
		Q_EMIT titleChanged();
	});
	d->dirModel->openUrl(d->dirNavigator->currentLocationUrl());
}

SDocument::~SDocument()
{

}

SWindow* SDocument::window() const
{
	return d->window;
}

QString SDocument::title() const
{
	return d->dirNavigator->currentLocationUrl().path();
}

KDirModel* SDocument::dirModel() const
{
	return d->dirModel;
}

KCoreUrlNavigator* SDocument::navigator() const
{
	return d->dirNavigator;
}

void SDocument::moveTo(SWindow* window)
{
	d->window->transferDocumentTo(this, window);
}
