#pragma once

#include <QAction>
#include <QObject>

class SMenuBar : public QObject
{

	Q_OBJECT

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SMenuBar(QObject* parent = nullptr);
	~SMenuBar();

	void about();
	void preferences();
	void emptyTrash();
	void newWindow();
	void newTab();
	void open();
	void getInfo();
	void closeWindow();
	void duplicate();
	void makeAlias();
	void moveToTrash();
	void undo();
	void cut();
	void copy();
	void paste();
	void selectAll();
	void viewAsIcons();
	void viewAsList();
	void togglePathBar();
	void toggleStatusBar();
	void toggleSidebar();
	void toggleToolbar();
	void customiseToolbar();
	void back();
	void forward();
	void up();
};
