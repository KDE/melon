#pragma once

#include <QQuickWindow>
#include <KDirModel>
#include <QItemSelectionModel>
#include <KIO/KCoreUrlNavigator>

#include "window.h"
#include "dirmodel.h"

class QQuickDropEvent;

class SDocument : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(Document)
	QML_UNCREATABLE("only c++ is allowed to make new documents")

	struct Private;
	QScopedPointer<Private> d;

	QString fancyPlacesTitle() const;
	QString fancyNameFor(const QUrl& url) const;
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

	Q_PROPERTY(QItemSelectionModel* selectionModel READ selectionModel CONSTANT)
	QItemSelectionModel* selectionModel() const;

	Q_PROPERTY(SDirModel* dirModel READ dirModel CONSTANT)
	SDirModel* dirModel() const;

	Q_PROPERTY(KCoreUrlNavigator* navigator READ navigator CONSTANT)
	KCoreUrlNavigator* navigator() const;

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

	Q_SIGNAL void pathSegmentChanged();

	Q_SIGNAL void statted();

	void getFileCounts();

	Q_INVOKABLE void copy();
	Q_INVOKABLE void cut();
	Q_INVOKABLE void paste();
	Q_INVOKABLE void startDrag();
	Q_INVOKABLE void moveTo(SWindow* window);
	Q_INVOKABLE void openItem(KFileItem item);
	Q_INVOKABLE void openRightClickMenuFor(KFileItem item);
	Q_INVOKABLE void openNewFileMenuFor(QQuickItem* item);
	Q_INVOKABLE void drop(QQuickItem* target, QQuickDropEvent* event);
	Q_INVOKABLE void openSelectedFiles();
	Q_INVOKABLE void duplicateSelectedFiles();
	Q_INVOKABLE void getInfoOnSelectedFiles();
	Q_INVOKABLE void aliasSelectedFiles();
	Q_INVOKABLE void trashSelectedFiles();

	// persistence functionality
	explicit SDocument(const KConfigGroup& config, SWindow* parent);
	void saveTo(KConfigGroup& configGroup) const;
	QUuid id() const;
};
