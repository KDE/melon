#pragma once

#include <QObject>
#include <QtQml>

#include "service.h"

class SPDocument : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(Document)

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SPDocument(const QUrl& url, QObject* parent = nullptr);
	~SPDocument();

	enum State
	{
		Loading,
		LoadingMimetypeKnown,
		Loaded,
		Failed,
	};
	Q_ENUM(State)

	Q_PROPERTY(QUrl url READ url CONSTANT)
	QUrl url() const;

	Q_PROPERTY(QString title READ title CONSTANT)
	QString title() const;

	Q_PROPERTY(State state READ state NOTIFY stateChanged)
	State state() const;
	Q_SIGNAL void stateChanged();

	Q_PROPERTY(QString failureString READ failureString NOTIFY failureStringChanged)
	QString failureString() const;
	Q_SIGNAL void failureStringChanged();

	Q_PROPERTY(QVariantList defaultApps READ defaultApps NOTIFY defaultAppsChanged)
	QVariantList defaultApps() const;
	Q_SIGNAL void defaultAppsChanged();

	Q_SIGNAL void closed();

	void instantiateComponent();
	void insertData();

	void opened();
	void mimeTypeFound(QMimeType mimeType);
	void dataLoaded(const QByteArray& data);
	void failed(const QString& uiString);
};
