// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QQuickWindow>
#include <KDirModel>
#include <QItemSelectionModel>
#include <KCoreUrlNavigator>
#include <QDropEvent>
#include <QQuickItem>

#include "columnsmodel.h"
#include "window.h"
#include "dirmodel.h"

class QQuickDropEventEx;
class SColumnsModel;

class SDocument : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(Document)
	QML_UNCREATABLE("only c++ is allowed to make new documents")

	struct Private;
	QScopedPointer<Private> d;

	QString fancyPlacesTitle() const;
	void recomputePathSegments();
	void preInit(SWindow* parent, const QUrl& in);
	void postInit();

public:
	explicit SDocument(SWindow* parent);
	explicit SDocument(const QUrl& in, SWindow* parent);
	~SDocument();

	Q_PROPERTY(SWindow* window READ window NOTIFY windowChanged)
	SWindow* window() const;
	Q_SIGNAL void windowChanged();

	Q_PROPERTY(QString title READ title NOTIFY titleChanged)
	QString title() const;
	Q_SIGNAL void titleChanged();
	Q_INVOKABLE QString fancyNameFor(const QUrl& url) const;

	Q_PROPERTY(SDirModel* dirModel READ dirModel CONSTANT)
	SDirModel* dirModel() const;

	Q_PROPERTY(KCoreUrlNavigator* navigator READ navigator CONSTANT)
	KCoreUrlNavigator* navigator() const;

	Q_PROPERTY(bool searchBarOpen READ searchBarOpen WRITE setSearchBarOpen NOTIFY searchBarOpenChanged)
	bool searchBarOpen();
	void setSearchBarOpen(bool open);
	Q_SIGNAL void searchBarOpenChanged();

	Q_PROPERTY(bool onlySearchingFromCurrentWorkingDirectory READ onlySearchingFromCurrentWorkingDirectory WRITE setOnlySearchingFromCurrentWorkingDirectory NOTIFY onlySearchingFromCurrentWorkingDirectoryChanged)
	bool onlySearchingFromCurrentWorkingDirectory();
	void setOnlySearchingFromCurrentWorkingDirectory(bool open);
	Q_SIGNAL void onlySearchingFromCurrentWorkingDirectoryChanged();

	Q_PROPERTY(bool searchIncludesContents READ searchIncludesContents WRITE setSearchIncludesContents NOTIFY searchIncludesContentsChanged)
	bool searchIncludesContents();
	void setSearchIncludesContents(bool includes);
	Q_SIGNAL void searchIncludesContentsChanged();

	Q_PROPERTY(QUrl actualViewingURL READ actualViewingURL NOTIFY actualViewingURLChanged)
	QUrl actualViewingURL();
	void recomputeActualViewingURL();
	QUrl computeSearchURL();
	Q_SIGNAL void actualViewingURLChanged();

	Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
	QString searchText();
	void setSearchText(const QString& text);
	Q_SIGNAL void searchTextChanged();

	Q_INVOKABLE void saveCurrentSearch();

	KFileItemList selectedFiles() const;
	QList<QUrl> selectedURLs() const;

	Q_PROPERTY(int numberOfFiles READ numberOfFiles NOTIFY statted)
	int numberOfFiles() const;

	Q_PROPERTY(int numberOfFolders READ numberOfFolders NOTIFY statted)
	int numberOfFolders() const;

	Q_PROPERTY(QString dirSize READ dirSize NOTIFY statted)
	QString dirSize() const;

	Q_PROPERTY(bool writable READ writable NOTIFY statted)
	bool writable() const;

	Q_PROPERTY(bool local READ local NOTIFY statted)
	bool local() const;

	Q_PROPERTY(QStringList pathSegmentStrings READ pathSegmentStrings NOTIFY pathSegmentChanged)
	QStringList pathSegmentStrings() const;

	Q_PROPERTY(QList<QUrl> pathSegmentURLs READ pathSegmentURLs NOTIFY pathSegmentChanged)
	QList<QUrl> pathSegmentURLs() const;

	Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
	bool loading() const;
	Q_SIGNAL void loadingChanged();

	Q_PROPERTY(SColumnsModel* columnsModel READ columnsModel CONSTANT)
	SColumnsModel* columnsModel() const;
	Q_INVOKABLE void openColumnUrl(int containingIndex, const QUrl& url);

	Q_SIGNAL void pathSegmentChanged();

	Q_SIGNAL void statted();

	void getFileCounts();

	Q_PROPERTY(QItemSelectionModel* rawSelectionModel READ rawSelectionModel CONSTANT)
	QItemSelectionModel* rawSelectionModel() const;

	Q_PROPERTY(KFileItem viewingFileItem READ viewingFileItem WRITE setViewingFileItem RESET resetViewingFileItem NOTIFY viewingFileItemChanged)
	KFileItem viewingFileItem() const;
	void setViewingFileItem(KFileItem item);
	void resetViewingFileItem();
	Q_SIGNAL void viewingFileItemChanged();

	SDirModel* currentDirModel() const;
	Q_INVOKABLE QItemSelectionModel* selectionModelFor(SDirModel* model) const;
	Q_INVOKABLE void copy();
	Q_INVOKABLE void cut();
	Q_INVOKABLE void paste();
	Q_INVOKABLE void startDrag();
	Q_INVOKABLE void moveTo(SWindow* window);
	Q_INVOKABLE void openUrl(const QUrl& url);
	Q_INVOKABLE void openItem(KFileItem item);
	Q_INVOKABLE void openRightClickMenuFor(KFileItem item, SDirModel* model);
	Q_INVOKABLE void openNewFileMenuFor(QQuickItem* item);
	Q_INVOKABLE void drop(QQuickItem* target, QDropEvent* event, const QUrl& url = {});
	Q_INVOKABLE void openSelectedFiles();
	Q_INVOKABLE void duplicateSelectedFiles();
	Q_INVOKABLE void getInfoOnSelectedFiles();
	Q_INVOKABLE void getInfoOnFile(KFileItem item);
	Q_INVOKABLE void aliasSelectedFiles();
	Q_INVOKABLE void trashSelectedFiles();

	// persistence functionality
	explicit SDocument(const KConfigGroup& config, SWindow* parent);
	void saveTo(KConfigGroup& configGroup) const;
	QUuid id() const;
};
