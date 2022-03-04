#pragma once

#include <QQuickWindow>
#include <KDirModel>
#include <KIO/KCoreUrlNavigator>

class SDocument : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(Document)
	QML_UNCREATABLE("only c++ is allowed to make new documents")

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SDocument(QObject* parent = nullptr);
	~SDocument();

	Q_PROPERTY(QString title READ title NOTIFY titleChanged)
	QString title() const;
	Q_SIGNAL void titleChanged();

	Q_PROPERTY(KDirModel* dirModel READ dirModel CONSTANT)
	KDirModel* dirModel() const;

	Q_PROPERTY(KCoreUrlNavigator* navigator READ navigator CONSTANT)
	KCoreUrlNavigator* navigator() const;
};
