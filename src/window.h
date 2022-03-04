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
	explicit SWindow(QQuickWindow* window, SDocument* containing, QObject* parent = nullptr);
	~SWindow();

	Q_PROPERTY(SDocument* document READ document CONSTANT)
	SDocument* document() const;
	Q_SIGNAL void titleChanged();

	Q_PROPERTY(QQuickWindow* displayedIn READ displayedIn NOTIFY displayedInChanged)
	QQuickWindow* displayedIn() const;
	Q_SIGNAL void displayedInChanged();

};
