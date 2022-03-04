#include <QMenu>
#include <KFileItemActions>
#include <KFileItemListProperties>

#include "document.h"

struct SDocument::Private
{
	SWindow* window;
	KDirModel* dirModel;
	KCoreUrlNavigator* dirNavigator;
	KFileItemActions* fileItemActions;
};

SDocument::SDocument(SWindow* parent) : QObject(parent), d(new Private)
{
	d->window = parent;
	d->dirModel = new KDirModel(this);
	d->dirNavigator = new KCoreUrlNavigator(QUrl::fromLocalFile("/etc"), this);
	d->fileItemActions = new KFileItemActions(this);
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

void SDocument::openItem(KFileItem item)
{
	d->fileItemActions->setItemListProperties(KFileItemListProperties(KFileItemList{item}));
	d->fileItemActions->runPreferredApplications(KFileItemList{item});
}

void SDocument::openRightClickMenuFor(KFileItem item)
{
	QPointer<QMenu> menu(new QMenu);

	d->fileItemActions->setItemListProperties(KFileItemListProperties(KFileItemList{item}));
	d->fileItemActions->insertOpenWithActionsTo(nullptr, menu, QStringList{});
	d->fileItemActions->addActionsTo(menu);
	menu->winId();
	menu->windowHandle()->setTransientParent(d->window->displayedIn());
	menu->exec(QCursor::pos());

	if (menu) {
		menu->deleteLater();
	}
}
