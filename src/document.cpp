#include "document.h"

struct SDocument::Private
{
	KDirModel* dirModel;
	KCoreUrlNavigator* dirNavigator;
};

SDocument::SDocument(QObject* parent) : QObject(parent), d(new Private)
{
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
