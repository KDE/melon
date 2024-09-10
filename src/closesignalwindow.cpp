#include <QQuickItem>
#include <QQuickWidget>
#include "closesignalwindow.h"
#include "app.h"

using namespace Qt::StringLiterals;

struct SCloseSignalWindow::Private
{
	QQuickItem *content = nullptr;
};

SCloseSignalWindow::SCloseSignalWindow(QWidget *parent) : QMainWindow(parent)
{
	d.reset(new Private);
}

SCloseSignalWindow::~SCloseSignalWindow()
{
}

void SCloseSignalWindow::closeEvent(QCloseEvent *event)
{
	Q_UNUSED(event)
	Q_EMIT closing();
}

void SCloseSignalWindow::init(QQuickItem *content)
{
	setMenuBar(sApp->sMenuBar->createMenuBarFor(this));
	d->content = content;

	auto view = new QQuickWidget(sApp->engine.get(), this);
	view->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
	view->setSource(QUrl(u"qrc:/QuickWidgetWrapper.qml"_s));
	content->setParentItem(view->rootObject());
	view->rootObject()->setProperty("child", QVariant::fromValue(content));
	setCentralWidget(view);

	setWindowTitle(content->property("title").toString());
	connect(content, SIGNAL(titleChanged()), this, SLOT(viewTitleChanged()));
}

void SCloseSignalWindow::viewTitleChanged()
{
	setWindowTitle(d->content->property("title").toString());
}
