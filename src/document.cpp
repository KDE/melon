#include <QMenu>
#include <QDrag>
#include <QStandardPaths>
#include <KFileItemActions>
#include <KFileItemListProperties>
#include <KStandardAction>
#include <QItemSelectionModel>
#include <QGuiApplication>
#include <QClipboard>
#include <QMimeDatabase>
#include <KLocalizedString>
#include <KIO/PasteJob>
#include <KIO/StatJob>
#include <KIO/CopyJob>
#include <KIO/FileUndoManager>
#include <KFormat>

#include "app.h"
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

	QStringList pathSegmentStrings;
	QList<QUrl> pathSegmentURLs;
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
		recomputePathSegments();
		getFileCounts();
	});
	d->dirModel->openUrl(d->dirNavigator->currentLocationUrl());
	recomputePathSegments();
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

void SDocument::recomputePathSegments()
{
	const auto url = [u = d->dirNavigator->currentLocationUrl()](int index) {
		if (index < 0) {
			index = 0;
		}

		QUrl url = u;
		QString path = url.path();
		if (!path.isEmpty()) {
			if (index == 0) {
#ifdef Q_OS_WIN
				path = path.length() > 1 ? path.left(2) : QDir::rootPath();
#else
				path = "/";
#endif
			} else {
				path = path.section('/', 0, index);
			}
		}

		url.setPath(path);
		return url;
	};

	d->pathSegmentStrings.clear();
	d->pathSegmentURLs.clear();

	QUrl currentUrl = d->dirNavigator->currentLocationUrl();
	if (!currentUrl.isValid()) {
		Q_EMIT pathSegmentChanged();
		return;
	}

	const QString path = currentUrl.path();
	int idx = 0;
	bool hasNext = true;
	do {
		const bool isFirst = idx == 0;
		const QString dirName = path.section('/', idx, idx);
		hasNext = isFirst || !dirName.isEmpty();

		const auto dirUrl = url(idx);
		d->pathSegmentURLs << dirUrl;
		d->pathSegmentStrings << fancyNameFor(dirUrl);
		idx++;
	} while (hasNext);
	d->pathSegmentStrings.takeLast();
	d->pathSegmentURLs.takeLast();

	Q_EMIT pathSegmentChanged();
}

QStringList SDocument::pathSegmentStrings() const
{
	return d->pathSegmentStrings;
}

QList<QUrl> SDocument::pathSegmentURLs() const
{
	return d->pathSegmentURLs;
}

// this logic yoinked from dolphin,
// copyright (TODO find who wrote that code)
QString SDocument::fancyNameFor(const QUrl& url) const
{
	auto* placesModel = SApp::instance->placesModel();
	const auto pattern = url.adjusted(QUrl::StripTrailingSlash).toString(QUrl::FullyEncoded).append("/?");
	const auto& matchedPlaces = placesModel->match(
		placesModel->index(0, 0),
		KFilePlacesModel::UrlRole,
		QRegularExpression::anchoredPattern(pattern),
		1,
		Qt::MatchRegularExpression
	);

    if (!matchedPlaces.isEmpty()) {
        return placesModel->text(matchedPlaces.first());
    }

    if (!url.isLocalFile()) {
        QUrl adjustedUrl = url.adjusted(QUrl::StripTrailingSlash);
        QString caption;
        if (!adjustedUrl.fileName().isEmpty()) {
            caption = adjustedUrl.fileName();
        } else if (!adjustedUrl.path().isEmpty() && adjustedUrl.path() != "/") {
            caption = adjustedUrl.path();
        } else if (!adjustedUrl.host().isEmpty()) {
            caption = adjustedUrl.host();
        } else {
            caption = adjustedUrl.toString();
        }
        return caption;
    }

    QString fileName = url.adjusted(QUrl::StripTrailingSlash).fileName();
    if (fileName.isEmpty()) {
        fileName = '/';
    }

    return fileName;
}


QString SDocument::fancyPlacesTitle() const
{
	return fancyNameFor(d->dirNavigator->currentLocationUrl());
}

QString SDocument::title() const
{
	return fancyPlacesTitle();
	// return d->dirNavigator->currentLocationUrl().path();
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

void SDocument::startDrag()
{
	if (d->selectionModel->selectedIndexes().empty()) {
		return;
	}
	auto mdata = d->dirModel->mimeData(d->selectionModel->selectedIndexes());
	if (!mdata) {
		return;
	}

	// TODO: better pixmap
	auto pixmap = d->dirModel->data(d->selectionModel->selectedIndexes().first(), Qt::DecorationRole).value<QIcon>().pixmap(48);

	QDrag* drag = new QDrag(d->window);
	drag->setPixmap(pixmap);
	const QPoint hotSpot((pixmap.width() / pixmap.devicePixelRatio()) / 2, 0);
	drag->setHotSpot(hotSpot);
	drag->setMimeData(mdata);
	drag->exec(Qt::MoveAction | Qt::CopyAction | Qt::LinkAction, Qt::CopyAction);
}

void SDocument::paste()
{
	KIO::paste(QGuiApplication::clipboard()->mimeData(), d->dirNavigator->currentLocationUrl());
}

QItemSelectionModel* SDocument::selectionModel() const
{
	return d->selectionModel;
}

KFileItemList SDocument::selectedFiles() const
{
	auto indexes = d->selectionModel->selectedIndexes();
	if (indexes.isEmpty())
		return {};

	KFileItemList items;
	items.reserve(indexes.size());
	for (const auto& index : indexes) {
		items << d->dirModel->data(index, KDirModel::FileItemRole).value<KFileItem>();
	}

	return items;
}

QList<QUrl> SDocument::selectedURLs() const
{
	auto indexes = d->selectionModel->selectedIndexes();
	if (indexes.isEmpty())
		return {};

	QList<QUrl> items;
	items.reserve(indexes.size());
	for (const auto& index : indexes) {
		items << d->dirModel->data(index, KDirModel::FileItemRole).value<KFileItem>().url();
	}

	return items;
}

void SDocument::openSelectedFiles()
{
	const auto files = selectedFiles();
	if (files.isEmpty())
		return;
	d->fileItemActions->runPreferredApplications(files);
}

// this logic yoinked from dolphin,
// copyright (TODO find who wrote that code)
// TODO: select duplicated files
void SDocument::duplicateSelectedFiles()
{
	const auto files = selectedFiles();
	if (files.isEmpty())
		return;

    const QMimeDatabase db;

    // Duplicate all selected items and append "copy" to the end of the file name
    // but before the filename extension, if present
    for (const auto &item : files) {
        const QUrl originalURL  = item.url();
        const QString originalDirectoryPath = originalURL.adjusted(QUrl::RemoveFilename).path();
        const QString originalFileName = item.name();

        QString extension = db.suffixForFileName(originalFileName);

        QUrl duplicateURL = originalURL;

        // No extension; new filename is "<oldfilename> copy"
        if (extension.isEmpty()) {
            duplicateURL.setPath(originalDirectoryPath + i18nc("<filename> copy", "%1 copy", originalFileName));
        // There's an extension; new filename is "<oldfilename> copy.<extension>"
        } else {
            // Need to add a dot since QMimeDatabase::suffixForFileName() doesn't include it
            extension = QLatin1String(".") + extension;
            const QString originalFilenameWithoutExtension = originalFileName.chopped(extension.size());
            // Preserve file's original filename extension in case the casing differs
            // from what QMimeDatabase::suffixForFileName() returned
            const QString originalExtension = originalFileName.right(extension.size());
            duplicateURL.setPath(originalDirectoryPath + i18nc("<filename> copy", "%1 copy", originalFilenameWithoutExtension) + originalExtension);
        }

        KIO::CopyJob* job = KIO::copyAs(originalURL, duplicateURL);

        if (job) {
            KIO::FileUndoManager::self()->recordCopyJob(job);
        }
    }
}


// this logic yoinked from dolphin,
// copyright (TODO find who wrote that code)
// TODO: select duplicated files
void SDocument::aliasSelectedFiles()
{
	const auto files = selectedFiles();
	if (files.isEmpty())
		return;

    const QMimeDatabase db;

    // Duplicate all selected items and append "copy" to the end of the file name
    // but before the filename extension, if present
    for (const auto &item : files) {
        const QUrl originalURL  = item.url();
        const QString originalDirectoryPath = originalURL.adjusted(QUrl::RemoveFilename).path();
        const QString originalFileName = item.name();

        QString extension = db.suffixForFileName(originalFileName);

        QUrl aliasURL = originalURL;

        // No extension; new filename is "<oldfilename> alias"
        if (extension.isEmpty()) {
            aliasURL.setPath(originalDirectoryPath + i18nc("<filename> alias", "%1 alias", originalFileName));
        // There's an extension; new filename is "<oldfilename> alias.<extension>"
        } else {
            // Need to add a dot since QMimeDatabase::suffixForFileName() doesn't include it
            extension = QLatin1String(".") + extension;
            const QString originalFilenameWithoutExtension = originalFileName.chopped(extension.size());
            // Preserve file's original filename extension in case the casing differs
            // from what QMimeDatabase::suffixForFileName() returned
            const QString originalExtension = originalFileName.right(extension.size());
            aliasURL.setPath(originalDirectoryPath + i18nc("<filename> alias", "%1 alias", originalFilenameWithoutExtension) + originalExtension);
        }

        KIO::CopyJob* job = KIO::linkAs(originalURL, aliasURL);

        if (job) {
            KIO::FileUndoManager::self()->recordCopyJob(job);
        }
    }
}

void SDocument::trashSelectedFiles()
{
	const auto files = selectedURLs();
	KIO::Job* job = KIO::trash(files);
	if (job) {
		KIO::FileUndoManager::self()->recordJob(KIO::FileUndoManager::Trash, files, QUrl("trash:/"), job);
	}
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
