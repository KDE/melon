#include <QMenu>
#include <QStandardPaths>
#include <KFileItemActions>
#include <KFileItemListProperties>
#include <KStandardAction>
#include <QItemSelectionModel>
#include <QGuiApplication>
#include <QClipboard>
#include <KIO/PasteJob>
#include <KIO/StatJob>
#include <KFormat>

#include "document.h"

struct SDocument::Private
{
	SWindow* window;
	KDirModel* dirModel;
	KCoreUrlNavigator* dirNavigator;
	KFileItemActions* fileItemActions;
	QItemSelectionModel* selectionModel;
	KIO::StatJob* fileCountsJob;

	int folderCount = 0;
	int fileCount = 0;
	QString totalFileSize = 0;

	bool writable = true;
	bool local = true;
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
		getFileCounts();
	});
	d->dirModel->openUrl(d->dirNavigator->currentLocationUrl());
	getFileCounts();
}

int SDocument::numberOfFiles() const
{
	return d->fileCount;
}

int SDocument::numberOfFolders() const
{
	return d->folderCount;
}

void SDocument::getFileCounts()
{
	d->fileCountsJob = KIO::statDetails(
		d->dirNavigator->currentLocationUrl(),
		KIO::StatJob::SourceSide, KIO::StatRecursiveSize, KIO::HideProgressInfo
	);
	connect(d->fileCountsJob, &KJob::result, this, [job = d->fileCountsJob, this]() {
		const auto entry = job->statResult();
		int folderCount = 0;
		int fileCount = 0;
		KIO::filesize_t totalFileSize = 0;
		bool fileSizeCountNeeded = true;

		if (entry.contains(KIO::UDSEntry::UDS_RECURSIVE_SIZE)) {
			totalFileSize = static_cast<KIO::filesize_t>(entry.numberValue(KIO::UDSEntry::UDS_RECURSIVE_SIZE));
			fileSizeCountNeeded = false;
		}

		const int itemCount = d->dirModel->rowCount();
		for (int i = 0; i < itemCount; ++i) {
			const auto item = d->dirModel->data(d->dirModel->index(i, 0), KDirModel::FileItemRole).value<KFileItem>();

			if (item.isDir()) {
				folderCount++;
			} else {
				fileCount++;
				if (fileSizeCountNeeded) {
					totalFileSize += item.size();
				}
			}
		}

		KFileItem item(entry, d->dirNavigator->currentLocationUrl());
		item.isWritable();
		item.isLocalFile();

		d->totalFileSize = KFormat().formatByteSize(totalFileSize);
		d->fileCount = fileCount;
		d->folderCount = folderCount;
		d->writable = item.isWritable();
		d->local = !item.isSlow();
		Q_EMIT statted();
	});
}

SDocument::~SDocument()
{

}

bool SDocument::writable() const
{
	return d->writable;
}

QString SDocument::dirSize() const
{
	return d->totalFileSize;
}

bool SDocument::local() const
{
	return d->local;
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
