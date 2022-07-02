#pragma once

#include <QObject>

class SPApp : public QObject
{

	Q_OBJECT

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SPApp(QObject* parent = nullptr);
	~SPApp();

	// adaptor implementations
	Q_INVOKABLE void ShowFile(const QString &uri, const QString &parentWindowHandle);
};
