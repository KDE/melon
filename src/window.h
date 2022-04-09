#pragma once

#include <QQuickWindow>
#include <KDirModel>
#include <KIO/KCoreUrlNavigator>
#include <KConfigGroup>

class SDocument;

class SWindow : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(Window)
	QML_UNCREATABLE("only c++ is allowed to make new windows")

	struct Private;
	QScopedPointer<Private> d;

	void init();

public:
	explicit SWindow(QQuickWindow* window, QObject* parent = nullptr);
	explicit SWindow(const QUrl& in, QQuickWindow* window, QObject* parent = nullptr);
	~SWindow();

	Q_PROPERTY(QList<SDocument*> documents READ documents NOTIFY documentsChanged)
	QList<SDocument*> documents() const;
	Q_SIGNAL void documentsChanged();

	Q_INVOKABLE void closeDocument(int idx);
	Q_INVOKABLE void newDocument();

	Q_PROPERTY(QQuickWindow* displayedIn READ displayedIn NOTIFY displayedInChanged)
	QQuickWindow* displayedIn() const;
	Q_SIGNAL void displayedInChanged();

	Q_PROPERTY(SDocument* activeDocument READ activeDocument WRITE setActiveDocument NOTIFY activeDocumentChanged)
	SDocument* activeDocument() const;
	void setActiveDocument(SDocument* document);
	Q_SIGNAL void activeDocumentChanged();

	Q_SIGNAL void closing(SWindow* self);

	Q_INVOKABLE void transferDocumentTo(SDocument* document, SWindow* window);

	// persistence functionality
	explicit SWindow(const KConfigGroup& config, QQuickWindow* window, QObject* parent = nullptr);
	void afterComponentComplete(const KConfigGroup& config);
	void saveTo(KConfigGroup& configGroup) const;
	QUuid id() const;
};
