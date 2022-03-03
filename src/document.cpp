#include "document.h"

struct SDocument::Private
{
	QQuickWindow* displayedIn;
	KDirModel* dirModel;
	KCoreUrlNavigator* dirNavigator;
};

SDocument::SDocument(QQuickWindow* displayedIn, QObject* parent) : QObject(parent), d(new Private)
{
	d->displayedIn = displayedIn;
	d->dirModel = new KDirModel(this);
	d->dirNavigator = new KCoreUrlNavigator(QUrl::fromLocalFile("/etc"), this);
	connect(d->dirNavigator, &KCoreUrlNavigator::currentLocationUrlChanged, this, [this]() {
		d->dirModel->openUrl(d->dirNavigator->currentLocationUrl());
	});
	d->dirModel->openUrl(d->dirNavigator->currentLocationUrl());
}

SDocument::~SDocument()
{

}

QQuickWindow* SDocument::displayedIn() const
{
	return d->displayedIn;
}

KDirModel* SDocument::dirModel() const
{
	return d->dirModel;
}

KCoreUrlNavigator* SDocument::navigator() const
{
	return d->dirNavigator;
}
