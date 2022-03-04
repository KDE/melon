#pragma once

#include <QQuickWindow>
#include <KDirModel>
#include <KIO/KCoreUrlNavigator>

#include "window.h"

class SDocument : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(Document)
	QML_UNCREATABLE("only c++ is allowed to make new documents")

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SDocument(SWindow* parent);
	~SDocument();

	Q_PROPERTY(SWindow* window READ window NOTIFY windowChanged)
	SWindow* window() const;
	Q_SIGNAL void windowChanged();

	Q_PROPERTY(QString title READ title NOTIFY titleChanged)
	QString title() const;
	Q_SIGNAL void titleChanged();

	Q_PROPERTY(KDirModel* dirModel READ dirModel CONSTANT)
	KDirModel* dirModel() const;

	Q_PROPERTY(KCoreUrlNavigator* navigator READ navigator CONSTANT)
	KCoreUrlNavigator* navigator() const;

	Q_INVOKABLE void moveTo(SWindow* window);
};
