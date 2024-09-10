#pragma once

#include <QMainWindow>
#include <QCloseEvent>
class QQuickItem;

class SCloseSignalWindow : public QMainWindow {
	Q_OBJECT

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SCloseSignalWindow(QWidget *parent = nullptr);
	~SCloseSignalWindow();

	void init(QQuickItem *content);
	Q_SIGNAL void closing();
	Q_SLOT void viewTitleChanged();

protected:
	void closeEvent(QCloseEvent *event) override;
};
