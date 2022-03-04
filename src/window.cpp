#include "window.h"

struct SWindow::Private
{
	QQuickWindow* displayedIn;
	SDocument* containing;
};

SWindow::SWindow(QQuickWindow* displayedIn, SDocument* containing, QObject* parent) : QObject(parent), d(new Private)
{
    d->displayedIn = displayedIn;
    d->containing = containing;
}

SWindow::~SWindow()
{

}

SDocument* SWindow::document() const
{
	return d->containing;
}

QQuickWindow* SWindow::displayedIn() const
{
	return d->displayedIn;
}
