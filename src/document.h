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
	explicit SDocument(QQuickWindow* window, QObject* parent = nullptr);
	~SDocument();

	Q_PROPERTY(QQuickWindow* displayedIn READ displayedIn NOTIFY displayedInChanged)
	QQuickWindow* displayedIn() const;
	Q_SIGNAL void displayedInChanged();

	Q_PROPERTY(KDirModel* dirModel READ dirModel CONSTANT)
	KDirModel* dirModel() const;

	Q_PROPERTY(KCoreUrlNavigator* navigator READ navigator CONSTANT)
	KCoreUrlNavigator* navigator() const;
};
