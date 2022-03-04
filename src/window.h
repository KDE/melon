#pragma once

#include <QQuickWindow>
#include <KDirModel>
#include <KIO/KCoreUrlNavigator>

class SDocument;

class SWindow : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(Window)
	QML_UNCREATABLE("only c++ is allowed to make new windows")

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SWindow(QQuickWindow* window, QObject* parent = nullptr);
	~SWindow();

	Q_PROPERTY(QList<SDocument*> documents READ documents NOTIFY documentsChanged)
	QList<SDocument*> documents() const;
	Q_SIGNAL void documentsChanged();

    Q_INVOKABLE void closeDocument(int idx);
    Q_INVOKABLE void newDocument();

	Q_PROPERTY(QQuickWindow* displayedIn READ displayedIn NOTIFY displayedInChanged)
	QQuickWindow* displayedIn() const;
	Q_SIGNAL void displayedInChanged();

    Q_INVOKABLE void transferDocumentTo(SDocument* document, SWindow* window);

};
