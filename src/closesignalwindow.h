#pragma once

#include <QMainWindow>
#include <QCloseEvent>

class SCloseSignalWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit SCloseSignalWindow(QWidget *parent = nullptr);
	~SCloseSignalWindow();

	Q_SIGNAL void closing();

protected:
	void closeEvent(QCloseEvent *event) override;
};
