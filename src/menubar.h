#pragma once

#include <QAction>
#include <QObject>
#include <KBookmarkOwner>
#include <QMenuBar>

class SMenuBar : public QObject, public KBookmarkOwner
{

	Q_OBJECT

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SMenuBar(QObject* parent = nullptr);
	~SMenuBar();

	QMenuBar *createMenuBarFor(QWidget *parent);

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
	void viewAsColumns();
	void togglePathBar();
	void toggleStatusBar();
	void toggleSidebar();
	void toggleToolbar();
	void customiseToolbar();
	void back();
	void forward();
	void up();

	QString currentTitle() const override;
	QUrl currentUrl() const override;
	bool enableOption(BookmarkOption option) const override;
	void openBookmark(const KBookmark& bookmark, Qt::MouseButtons, Qt::KeyboardModifiers) override;
};
