#include <QQuickWidget>
#include <QQuickItem>

#include "windowrestorer.h"
#include "window.h"
#include "app.h"

using namespace Qt::StringLiterals;

SWindowRestorer::SWindowRestorer() : QObject(), NGRestorer()
{
}

SWindowRestorer::~SWindowRestorer()
{
}

void SWindowRestorer::restore(QUuid id, const KConfigGroup& state, CompletionHandler completionHandler)
{
	auto win = qobject_cast<QQuickItem*>(sApp->windowComponent->beginCreate(sApp->engine->rootContext()));
	qWarning().noquote() << sApp->windowComponent->errorString();

	auto closeWindow = new SCloseSignalWindow();
	closeWindow->setMenuBar(sApp->sMenuBar->createMenuBarFor(closeWindow));

	auto view = new QQuickWidget(sApp->engine.get(), closeWindow);
	view->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
	view->setSource(QUrl(u"qrc:/QuickWidgetWrapper.qml"_s));
	win->setParentItem(view->rootObject());
	view->rootObject()->setProperty("child", QVariant::fromValue(win));
	closeWindow->setCentralWidget(view);

	auto window = new SWindow(id, state, closeWindow, sApp->engine.get());

	closeWindow->show();

	sApp->windowComponent->setInitialProperties(win, {{u"window"_s, QVariant::fromValue(window)}});
	sApp->windowComponent->completeCreate();

	window->afterComponentComplete(state);

	sApp->windows << window;
	QObject::connect(window, &SWindow::closing, sApp, &SApp::windowClosing);

	completionHandler(window);
}

static NGMetaTypeRegisterer<SWindowRestorer*> registerer;
