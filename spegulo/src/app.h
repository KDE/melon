#pragma once

#include <QObject>
#include <KIO/FileJob>
#include <KSyntaxHighlighting/repository.h>

class QQmlComponent;

struct Components
{
	QQmlComponent* window;

	QQmlComponent* plaintextViewer;
	QQmlComponent* unsupportedViewer;
};

class SPApp : public QObject
{

	Q_OBJECT

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SPApp(QObject* parent = nullptr);
	~SPApp();

	static SPApp* instance();

	const Components& components() const;
	KSyntaxHighlighting::Repository* syntaxHighlightingRepository() const;

	void fileOpen(KIO::FileJob* job);
	void fileData(KIO::FileJob* job, const QByteArray& data);
	void fileMimeTypeFound(KIO::FileJob* job, const QString& mimeType);
	void fileResult(KIO::FileJob* job);

	// adaptor implementations
	Q_INVOKABLE void ShowFile(const QString &uri, const QString &parentWindowHandle);
};

#define spApp (SPApp::instance())
