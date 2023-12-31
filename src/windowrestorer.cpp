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
	auto win = qobject_cast<QQuickWindow*>(sApp->windowComponent->beginCreate(sApp->engine->rootContext()));
	qWarning().noquote() << sApp->windowComponent->errorString();

	auto window = new SWindow(id, state, win, sApp->engine.get());

	sApp->windowComponent->setInitialProperties(win, {{u"window"_s, QVariant::fromValue(window)}});
	sApp->windowComponent->completeCreate();

	window->afterComponentComplete(state);

	sApp->windows << window;
	QObject::connect(window, &SWindow::closing, sApp, &SApp::windowClosing);

	completionHandler(window);
}

static NGMetaTypeRegisterer<SWindowRestorer*> registerer;
