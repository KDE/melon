#include <QMenu>
#include <QStandardPaths>
#include <KFileItemActions>
#include <KFileItemListProperties>
#include <KStandardAction>
#include <QItemSelectionModel>
#include <QGuiApplication>
#include <QClipboard>
#include <KIO/PasteJob>

#include "document.h"

struct SDocument::Private
{
	SWindow* window;
	KDirModel* dirModel;
	KCoreUrlNavigator* dirNavigator;
	KFileItemActions* fileItemActions;
	QItemSelectionModel* selectionModel;
};

SDocument::SDocument(SWindow* parent) :
	SDocument(QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)), parent)
{
}

SDocument::SDocument(const QUrl& in, SWindow* parent) : QObject(parent), d(new Private)
{
	d->window = parent;
	d->dirModel = new KDirModel(this);
	d->dirNavigator = new KCoreUrlNavigator(in, this);
	d->fileItemActions = new KFileItemActions(this);
	d->selectionModel = new QItemSelectionModel(d->dirModel, this);
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

void SDocument::copy()
{
	auto cboard = QGuiApplication::clipboard();
	cboard->setMimeData(d->dirModel->mimeData(d->selectionModel->selectedIndexes()));
}

void SDocument::cut()
{
	// TODO: cut handling
}

void SDocument::paste()
{
	KIO::paste(QGuiApplication::clipboard()->mimeData(), d->dirNavigator->currentLocationUrl());
}

QItemSelectionModel* SDocument::selectionModel() const
{
	return d->selectionModel;
}

void SDocument::openRightClickMenuFor(KFileItem item)
{
	QPointer<QMenu> menu(new QMenu);

	auto copy = KStandardAction::copy(this, &SDocument::copy, this),
		 // cut = KStandardAction::cut(this, &SDocument::cut, this),
		 paste = KStandardAction::paste(this, &SDocument::paste, this);

	copy->setEnabled(d->selectionModel->hasSelection());
	menu->addAction(copy);
	// cut->setEnabled(d->selectionModel->hasSelection());
	// menu->addAction(cut);
	menu->addAction(paste);

	menu->addSeparator();

	d->fileItemActions->setItemListProperties(KFileItemListProperties(KFileItemList{item}));
	d->fileItemActions->insertOpenWithActionsTo(nullptr, menu, QStringList{});
	d->fileItemActions->addActionsTo(menu);
	menu->createWinId();
	menu->windowHandle()->setTransientParent(d->window->displayedIn());
	menu->exec(QCursor::pos());

	if (menu) {
		menu->deleteLater();
	}
}
