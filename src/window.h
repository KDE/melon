#pragma once

#include "NGAppMain.h"
#include "closesignalwindow.h"
#include <QWindow>
#include <KDirModel>
#include <KCoreUrlNavigator>
#include <KConfigGroup>
#include <QQmlEngine>
Q_MOC_INCLUDE("document.h")

#include <NGLib.h>

class SDocument;

class SWindow : public QObject, public NGSavable
{
	Q_OBJECT
	QML_NAMED_ELEMENT(Window)
	QML_UNCREATABLE("only c++ is allowed to make new windows")
	Q_INTERFACES(NGSavable)

	struct Private;
	QScopedPointer<Private> d;

	void init();

public:
	explicit SWindow(SCloseSignalWindow* window, QObject* parent = nullptr);
	explicit SWindow(const QUrl& in, SCloseSignalWindow* window, QObject* parent = nullptr);
	~SWindow();

	Q_PROPERTY(QList<SDocument*> documents READ documents NOTIFY documentsChanged)
	QList<SDocument*> documents() const;
	Q_SIGNAL void documentsChanged();

	Q_INVOKABLE void closeDocument(int idx);
	Q_INVOKABLE void newDocument();
	Q_INVOKABLE void newDocumentAtUrl(const QUrl& url);

	Q_PROPERTY(SCloseSignalWindow* displayedIn READ displayedIn NOTIFY displayedInChanged)
	SCloseSignalWindow* displayedIn() const;
	Q_SIGNAL void displayedInChanged();

	Q_PROPERTY(SDocument* activeDocument READ activeDocument WRITE setActiveDocument NOTIFY activeDocumentChanged)
	SDocument* activeDocument() const;
	void setActiveDocument(SDocument* document);
	Q_SIGNAL void activeDocumentChanged();

	Q_SIGNAL void closing(SWindow* self);

	Q_INVOKABLE void transferDocumentTo(SDocument* document, SWindow* window);

	// persistence functionality
	explicit SWindow(QUuid id, const KConfigGroup& config, SCloseSignalWindow* window, QObject* parent = nullptr);
	void afterComponentComplete(const KConfigGroup& config);

	NGSavable::SaveInformation save(KConfigGroup& configGroup) const override;
	QUuid identifier() const override;
};
