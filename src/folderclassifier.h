#pragma once

#include <QObject>
#include <QQmlEngine>

class FolderClassifier : public QObject
{

	Q_OBJECT
	QML_NAMED_ELEMENT(FolderClassifier)
	QML_SINGLETON

public:
	explicit FolderClassifier(QObject* parent = nullptr);
	~FolderClassifier();

	Q_INVOKABLE QString classifyFolder(const QUrl& url);
};