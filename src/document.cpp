// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "qnamespace.h"
#include <QMenu>
#include <QDrag>
#include <QTimer>
#include <QStandardPaths>
#include <KFileItemActions>
#include <KFileItemListProperties>
#include <Baloo/Query>
#include <KStandardAction>
#include <QItemSelectionModel>
#include <QGuiApplication>
#include <QClipboard>
#include <QMimeDatabase>
#include <KLocalizedString>
#include <QScopeGuard>
#include <KCoreDirLister>
#include <KDirLister>
#include <KIO/PasteJob>
#include <KIO/StatJob>
#include <KIO/CopyJob>
#include <KIO/DropJob>
#include <KIO/FileUndoManager>
#include <KNewFileMenu>
#include <KFormat>
#include <QQuickItem>
#include <QMimeData>
#include <KPropertiesDialog>
#include <QUrlQuery>
#include <optional>
#include <QRegularExpression>
#include <QDropEvent>

#include "app.h"
#include "document.h"
#include "columnsmodel_p.h"

using namespace Qt::StringLiterals;

struct SDocument::Private
{
	SWindow* window;
	SDirModel* dirModel;
	SColumnsModel* columnsModel;
	KCoreUrlNavigator* dirNavigator;
	KFileItemActions* fileItemActions;
	KIO::StatJob* fileCountsJob;
	KNewFileMenu* newFileMenu;
	QTimer* searchChangedTimer;
	QItemSelectionModel* selectionModel;
	KFileItem viewingFileItem;

	int folderCount = 0;
	int fileCount = 0;
	QString totalFileSize = QString();

	bool writable = true;
	bool local = true;
	bool loading = false;

	QStringList pathSegmentStrings;
	QList<QUrl> pathSegmentURLs;
	std::optional<QUrl> overlayURL;

	QUuid uuid;

	QString searchText;
	bool searchBarOpen = false;
	bool onlySearchingFromCurrentWorkingDirectory = true;
	bool searchIncludesContents = false;
};

void SDocument::preInit(SWindow* parent, const QUrl& in)
{
	d->window = parent;
	d->dirModel = new SDirModel(this);
	d->selectionModel = new QItemSelectionModel(d->dirModel, this);
	d->columnsModel = new SColumnsModel(this);
	connect(d->columnsModel, &SColumnsModel::columnsChanged, this, &SDocument::titleChanged);
	connect(d->selectionModel, &QItemSelectionModel::selectionChanged, this, [=] {
		if (d->selectionModel->selectedIndexes().length() < 2) {
			if (d->selectionModel->selectedIndexes().length() == 1) {
				setViewingFileItem(d->selectionModel->selectedIndexes().first().data(KDirModel::FileItemRole).value<KFileItem>());
			}
			return;
		}

		int idx = -1;
		int i = -1;
		for (const auto& model : d->columnsModel->d->dirModels) {
			i++;
			if (model.data() == d->selectionModel->model()) {
				idx = i;
				break;
			}
		}
		if (idx != -1) {
			d->columnsModel->beginRemoveRows(QModelIndex(), idx+1, d->columnsModel->d->dirModels.length());
			d->columnsModel->d->dirModels = d->columnsModel->d->dirModels.mid(0, idx+1);
			Q_EMIT d->columnsModel->columnsChanged();
		}
	});
	d->dirNavigator = new KCoreUrlNavigator(in, this);
	d->fileItemActions = new KFileItemActions(this);
	d->newFileMenu = new KNewFileMenu(this);
	d->searchChangedTimer = new QTimer(this);
	d->searchChangedTimer->setInterval(300);
	d->searchChangedTimer->setSingleShot(true);

	connect(d->searchChangedTimer, &QTimer::timeout, this, &SDocument::recomputeActualViewingURL);
	connect(d->dirNavigator, &KCoreUrlNavigator::currentLocationUrlChanged, this, &SDocument::recomputeActualViewingURL);
	connect(this, &SDocument::searchBarOpenChanged, this, &SDocument::recomputeActualViewingURL);
	connect(this, &SDocument::onlySearchingFromCurrentWorkingDirectoryChanged, this, &SDocument::recomputeActualViewingURL);
	connect(this, &SDocument::searchIncludesContentsChanged, this, &SDocument::recomputeActualViewingURL);
	connect(this, &SDocument::searchTextChanged, [this] {
		d->searchChangedTimer->start();
	});

	connect(this, &SDocument::actualViewingURLChanged, this, [this]() {
		d->dirModel->openUrl(actualViewingURL());
		Q_EMIT titleChanged();
		recomputePathSegments();
		getFileCounts();
	});
	connect(d->dirModel->dirLister(), &KCoreDirLister::started, this, [=] {
		d->loading = true;
		Q_EMIT loadingChanged();
	});
	connect(d->dirModel->dirLister(), &KCoreDirLister::listingDirCompleted, this, [=] {
		d->loading = false;
		Q_EMIT loadingChanged();
	});
	connect(d->dirModel->dirLister(), qOverload<>(&KCoreDirLister::canceled), this, [=] {
		d->loading = false;
		Q_EMIT loadingChanged();
	});
}

void SDocument::postInit()
{
	if (d->uuid.isNull())
		d->uuid = QUuid::createUuid();

	d->dirModel->openUrl(d->dirNavigator->currentLocationUrl());
	d->columnsModel->d->dirModels = {d->dirModel->duplicate()};
	recomputePathSegments();
	getFileCounts();
}

SDocument::SDocument(SWindow* parent) : SDocument(QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)), parent)
{
}

SDocument::SDocument(const QUrl& in, SWindow* parent) : QObject(parent), d(new Private)
{
	preInit(parent, in);
	postInit();
}

bool SDocument::searchBarOpen()
{
	return d->searchBarOpen;
}

void SDocument::setSearchBarOpen(bool open)
{
	if (open == d->searchBarOpen)
		return;

	d->searchBarOpen = open;
	Q_EMIT searchBarOpenChanged();
}

QUrl SDocument::actualViewingURL()
{
	if (d->overlayURL.has_value())
		return *d->overlayURL;

	return d->dirNavigator->currentLocationUrl();
}

QUrl SDocument::computeSearchURL()
{
	QUrl url;

	if (!d->searchIncludesContents) {
		url.setScheme(u"filenamesearch"_s);

		QUrlQuery query;
		query.addQueryItem(u"search"_s, d->searchText);
		if (d->onlySearchingFromCurrentWorkingDirectory) {
			query.addQueryItem(u"url"_s, d->dirNavigator->currentLocationUrl().url());
		}

		url.setQuery(query);
	} else {
		Baloo::Query query;

		query.setSearchString(d->searchText);
		query.setIncludeFolder(d->dirNavigator->currentLocationUrl().url());

		url = query.toSearchUrl();
	}

	return url;
}

void SDocument::openUrl(const QUrl& url)
{
	if (url.scheme() == u"filenamesearch"_s) {
		auto query = QUrlQuery(url);
		setSearchText(query.queryItemValue(u"search"_s));
		if (query.hasQueryItem(u"url"_s)) {
			d->dirNavigator->setCurrentLocationUrl(QUrl(query.queryItemValue(u"url"_s)));
			setOnlySearchingFromCurrentWorkingDirectory(true);
		}
		setSearchBarOpen(true);
	} else if (url.scheme() == u"baloosearch"_s) {
		auto query = Baloo::Query::fromSearchUrl(url);
		setSearchText(query.searchString());
		if (!query.includeFolder().isEmpty()) {
			d->dirNavigator->setCurrentLocationUrl(QUrl(query.includeFolder()));
			setOnlySearchingFromCurrentWorkingDirectory(true);
		}
		setSearchBarOpen(true);
	} else {
		d->dirNavigator->setCurrentLocationUrl(url);
		d->columnsModel->beginResetModel();
		d->columnsModel->d->dirModels = {d->dirModel->duplicate()};
		d->columnsModel->endResetModel();
		Q_EMIT d->columnsModel->columnsChanged();
	}
}

void SDocument::openColumnUrl(int containingIndex, const QUrl& url)
{
	auto scope = qScopeGuard([=] { d->dirNavigator->setCurrentLocationUrl(url); });

	// if it's at the end of the columns, then we always do a new one
	if (containingIndex == d->columnsModel->d->dirModels.length() - 1) {
		auto newModel = QSharedPointer<SDirModel>::create();
		newModel->openUrl(url);
		d->columnsModel->beginInsertRows(QModelIndex(), d->columnsModel->d->dirModels.length(), d->columnsModel->d->dirModels.length());
		d->columnsModel->d->dirModels << newModel;
		Q_EMIT d->columnsModel->columnsChanged();
		d->columnsModel->endInsertRows();
		return;
	}

	if (d->columnsModel->d->dirModels[containingIndex+1]->currentURL() == url) {
		return;
	}

	// otherwise we replace the one after this one if the url isn't the same
	auto newModel = QSharedPointer<SDirModel>::create();
	newModel->openUrl(url);

	// if we don't yoink the existing model, it'll get deleted before qml updates to the new one.
	// this is problematic, because qml engine will overwrite properties referring to it with null,
	// breaking the binding.
	// you'll see something like this in the console:
	//		qrc:/ColumnsFileView.qml: Writing to "modelData" broke the binding to the underlying model
	//
	// so we make sure it lives until after qml has been notified of the new stuff.
	auto yoinkedModel = d->columnsModel->d->dirModels[containingIndex+1];

	d->columnsModel->d->dirModels[containingIndex+1] = newModel;
	Q_EMIT d->columnsModel->dataChanged(d->columnsModel->index(containingIndex+1), d->columnsModel->index(containingIndex+1));

	// then we chop off the ones after the replaced column
	// we do this by making the list span only the columns to this one + one more
	d->columnsModel->beginRemoveRows(QModelIndex(), containingIndex+2, d->columnsModel->d->dirModels.length());
	d->columnsModel->d->dirModels = d->columnsModel->d->dirModels.mid(0, containingIndex+2);
	d->columnsModel->endRemoveRows();
	Q_EMIT d->columnsModel->columnsChanged();
}

void SDocument::recomputeActualViewingURL()
{
	std::optional<QUrl> newUrl;

	if (d->searchBarOpen && !d->searchText.isEmpty()) {
		newUrl = computeSearchURL();
	}

	d->overlayURL = newUrl;
	Q_EMIT actualViewingURLChanged();
	Q_EMIT titleChanged();

	const auto clurl = d->dirNavigator->currentLocationUrl();
	for (const auto& column : d->columnsModel->d->dirModels) {
		if (column->currentURL().matches(clurl, QUrl::StripTrailingSlash)) {
			return;
		}
	}

	d->columnsModel->beginResetModel();
	d->columnsModel->d->dirModels = {d->dirModel->duplicate()};
	d->columnsModel->endResetModel();
}

QString SDocument::searchText()
{
	return d->searchText;
}

void SDocument::setSearchText(const QString& text)
{
	if (d->searchText == text)
		return;

	d->searchText = text;
	Q_EMIT searchTextChanged();
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
	d->fileCountsJob = KIO::stat(
		actualViewingURL(),
		KIO::StatJob::SourceSide, KIO::StatRecursiveSize, KIO::HideProgressInfo);
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

		KFileItem item(entry, actualViewingURL());
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
				path = u"/"_s;
#endif
			} else {
				path = path.section(u'/', 0, index);
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
		const QString dirName = path.section(u'/', idx, idx);
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
	auto* placesModel = sApp->placesModel();
	const auto pattern = url.adjusted(QUrl::StripTrailingSlash).toString(QUrl::FullyEncoded).append(u"/?"_s);
	const auto& matchedPlaces = placesModel->match(
		placesModel->index(0, 0),
		KFilePlacesModel::UrlRole,
		QRegularExpression::anchoredPattern(pattern),
		1,
		Qt::MatchRegularExpression);

	if (!matchedPlaces.isEmpty()) {
		return placesModel->text(matchedPlaces.first());
	}

	if (!url.isLocalFile()) {
		QUrl adjustedUrl = url.adjusted(QUrl::StripTrailingSlash);
		QString caption;
		if (!adjustedUrl.fileName().isEmpty()) {
			caption = adjustedUrl.fileName();
		} else if (!adjustedUrl.path().isEmpty() && adjustedUrl.path() != u"/"_s) {
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
		fileName = u'/';
	}

	return fileName;
}


QString SDocument::fancyPlacesTitle() const
{
	return fancyNameFor(currentDirModel()->currentURL());
}

QString SDocument::title() const
{
	if (d->searchBarOpen) {
		return i18n("Search");
	}
	return fancyPlacesTitle();
}

SDirModel* SDocument::dirModel() const
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
	cboard->setMimeData(d->dirModel->mimeData(selectionModelFor(currentDirModel())->selectedIndexes()));
}

void SDocument::cut()
{
	// TODO: cut handling
}

SDirModel* SDocument::currentDirModel() const
{
	if (sApp->viewMode() == SApp::Columns) {
		return d->columnsModel->d->dirModels.last().data();
	} else {
		return d->dirModel;
	}
}

void SDocument::startDrag()
{
	if (selectionModelFor(currentDirModel())->selectedIndexes().empty()) {
		return;
	}
	auto mdata = d->dirModel->mimeData(selectionModelFor(currentDirModel())->selectedIndexes());
	if (!mdata) {
		return;
	}

	// TODO: better pixmap
	auto pixmap = d->dirModel->data(selectionModelFor(currentDirModel())->selectedIndexes().first(), Qt::DecorationRole).value<QIcon>().pixmap(48);

	QDrag* drag = new QDrag(d->window);
	drag->setPixmap(pixmap);
	const QPoint hotSpot((pixmap.width() / pixmap.devicePixelRatio()) / 2, 0);
	drag->setHotSpot(hotSpot);
	drag->setMimeData(mdata);
	drag->exec(Qt::MoveAction | Qt::CopyAction | Qt::LinkAction, Qt::CopyAction);
}

void SDocument::paste()
{
	KIO::paste(QGuiApplication::clipboard()->mimeData(), actualViewingURL());
}

KFileItemList SDocument::selectedFiles() const
{
	auto indexes = selectionModelFor(currentDirModel())->selectedIndexes();
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
	auto indexes = selectionModelFor(currentDirModel())->selectedIndexes();
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

void SDocument::getInfoOnSelectedFiles()
{
	auto dialog = new KPropertiesDialog(selectedFiles());
	dialog->createWinId();
	dialog->windowHandle()->setTransientParent(qGuiApp->focusWindow());
	dialog->show();
}

void SDocument::getInfoOnFile(KFileItem item)
{
	auto dialog = new KPropertiesDialog({item});
	dialog->createWinId();
	dialog->windowHandle()->setTransientParent(qGuiApp->focusWindow());
	dialog->show();
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
	for (const auto& item : files) {
		const QUrl originalURL = item.url();
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
	for (const auto& item : files) {
		const QUrl originalURL = item.url();
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
		KIO::FileUndoManager::self()->recordJob(KIO::FileUndoManager::Trash, files, QUrl(u"trash:/"_s), job);
	}
}

void SDocument::openNewFileMenuFor(QQuickItem* item)
{
	d->newFileMenu->setWorkingDirectory(actualViewingURL());
	d->newFileMenu->checkUpToDate();

	item->setProperty("down", true);

	auto menu = d->newFileMenu->menu();
	menu->createWinId();
	menu->windowHandle()->setTransientParent(d->window->displayedIn()->windowHandle());
	menu->exec(QCursor::pos());

	item->setProperty("down", QVariant());
}

void SDocument::openRightClickMenuFor(KFileItem item, SDirModel* model)
{
	QPointer<QMenu> menu(new QMenu);

	auto copy = KStandardAction::copy(this, &SDocument::copy, this),
		 // cut = KStandardAction::cut(this, &SDocument::cut, this),
		paste = KStandardAction::paste(this, &SDocument::paste, this);

	copy->setEnabled(selectionModelFor(model)->hasSelection());
	menu->addAction(i18n("Get Info"), this, [item] {
		auto dialog = new KPropertiesDialog(item);
		dialog->createWinId();
		dialog->windowHandle()->setTransientParent(qGuiApp->focusWindow());
		dialog->show();
	});
	menu->addAction(copy);
	// cut->setEnabled(d->dirModel->selectionModel()->hasSelection());
	// menu->addAction(cut);
	menu->addAction(paste);

	menu->addSeparator();

	d->fileItemActions->setItemListProperties(KFileItemListProperties(KFileItemList{item}));
	d->fileItemActions->insertOpenWithActionsTo(nullptr, menu, QStringList{});
	d->fileItemActions->addActionsTo(menu);
	menu->createWinId();
	menu->windowHandle()->setTransientParent(d->window->displayedIn()->windowHandle());
	menu->exec(QCursor::pos());

	if (menu) {
		menu->deleteLater();
	}
}

bool SDocument::loading() const
{
	return d->loading;
}

SColumnsModel* SDocument::columnsModel() const
{
	return d->columnsModel;
}

QItemSelectionModel* SDocument::rawSelectionModel() const
{
	return d->selectionModel;
}

QItemSelectionModel* SDocument::selectionModelFor(SDirModel* model) const
{
	if (d->selectionModel->model() != model) {
		d->selectionModel->setModel(model);
		d->selectionModel->clearSelection();

		int idx = -1;
		int i = -1;
		for (const auto& iteratingModel : d->columnsModel->d->dirModels) {
			i++;
			if (iteratingModel.data() == model) {
				idx = i;
				break;
			}
		}
		if (idx != -1) {
			d->columnsModel->beginRemoveRows(QModelIndex(), idx+2, d->columnsModel->d->dirModels.length());
			d->columnsModel->d->dirModels = d->columnsModel->d->dirModels.mid(0, idx+2);
			d->columnsModel->endRemoveRows();
			Q_EMIT d->columnsModel->columnsChanged();
		}
	}

    return d->selectionModel;
}

void SDocument::drop(QQuickItem* target, QDropEvent* event, const QUrl& url)
{
	qWarning() << "drop!" << event;

	const QMimeData* mimeData = event->mimeData();
	qWarning() << "mime data" << mimeData;
	if (mimeData == nullptr) {
		return;
	}

	KFileItem item = target->property("fileItem").value<KFileItem>();
	// const int x = event->x();
	// const int y = event->y();
	// const QPoint dropPos = {x, y};

	// auto proposedAction = event->proposedAction();
	// auto possibleActions = event->supportedActions();
	// auto buttons = event->event->mouseButtons();
	// auto modifiers = event->event->keyboardModifiers();

	// auto pos = target->mapToScene(dropPos).toPoint();
	// pos = target->window()->mapToGlobal(pos);
	// QDropEvent ev(pos, possibleActions, mimeData, buttons, modifiers);
	// ev.setDropAction(proposedAction);

	KIO::drop(event, item.isDir() ? item.url() : (url.isEmpty() ? actualViewingURL() : url));
}

SDocument::SDocument(const KConfigGroup& config, SWindow* parent) : QObject(parent), d(new Private)
{
	const auto homeDir = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

	preInit(parent, config.readEntry<QUrl>("currentUrl", homeDir));

	d->uuid = config.readEntry<QUuid>("id", QUuid::createUuid());
	d->searchText = config.readEntry<QString>("searchText", QString());
	d->searchBarOpen = config.readEntry<bool>("searchBarOpen", false);
	d->onlySearchingFromCurrentWorkingDirectory = config.readEntry<bool>("onlySearchingFromCurrentWorkingDirectory", true);
	d->searchIncludesContents = config.readEntry<bool>("searchIncludesContents", false);

	postInit();
}

void SDocument::saveTo(KConfigGroup& config) const
{
	config.writeEntry("currentUrl", d->dirNavigator->currentLocationUrl());
	config.writeEntry("id", d->uuid);
	config.writeEntry("searchText", d->searchText);
	config.writeEntry("searchBarOpen", d->searchBarOpen);
	config.writeEntry("onlySearchingFromCurrentWorkingDirectory", d->onlySearchingFromCurrentWorkingDirectory);
	config.writeEntry("searchIncludesContents", d->searchIncludesContents);
}

QUuid SDocument::id() const
{
	return d->uuid;
}

bool SDocument::onlySearchingFromCurrentWorkingDirectory()
{
	return d->onlySearchingFromCurrentWorkingDirectory;
}

void SDocument::setOnlySearchingFromCurrentWorkingDirectory(bool open)
{
	if (d->onlySearchingFromCurrentWorkingDirectory == open)
		return;

	d->onlySearchingFromCurrentWorkingDirectory = open;
	Q_EMIT onlySearchingFromCurrentWorkingDirectoryChanged();
}

bool SDocument::searchIncludesContents()
{
	return d->searchIncludesContents;
}

void SDocument::setSearchIncludesContents(bool includes)
{
	if (d->searchIncludesContents == includes)
		return;

	d->searchIncludesContents = includes;
	Q_EMIT searchIncludesContentsChanged();
}

void SDocument::saveCurrentSearch()
{
	const auto label = d->onlySearchingFromCurrentWorkingDirectory ?
		i18n("Search for %1 in %2", d->searchText, d->dirNavigator->currentLocationUrl().fileName()) :
		i18n("Search for %1", d->searchText);
	sApp->placesModel()->addPlace(label, computeSearchURL(), u"folder-saved-search-symbolic"_s);
}

KFileItem SDocument::viewingFileItem() const
{
	return d->viewingFileItem;
}

void SDocument::setViewingFileItem(KFileItem item)
{
	if (item == d->viewingFileItem)
		return;

	d->viewingFileItem = item;
	Q_EMIT viewingFileItemChanged();
}

void SDocument::resetViewingFileItem()
{
	setViewingFileItem(KFileItem());
}
