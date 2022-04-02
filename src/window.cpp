#include "window.h"
#include "document.h"

struct SWindow::Private
{
	QQuickWindow* displayedIn;
	QList<SDocument*> containing;
    SDocument* activeDocument = nullptr;
};

SWindow::SWindow(QQuickWindow* displayedIn, QObject* parent) : QObject(parent), d(new Private)
{
    d->displayedIn = displayedIn;
    newDocument();
}

SWindow::SWindow(const QUrl& in, QQuickWindow* displayedIn, QObject* parent) : QObject(parent), d(new Private)
{
    d->displayedIn = displayedIn;
    d->containing << new SDocument(in, this);
    Q_EMIT documentsChanged();
}

SWindow::~SWindow()
{

}

QList<SDocument*> SWindow::documents() const
{
	return d->containing;
}

QQuickWindow* SWindow::displayedIn() const
{
	return d->displayedIn;
}

SDocument* SWindow::activeDocument() const
{
    return d->activeDocument;
}

void SWindow::setActiveDocument(SDocument* document)
{
    if (d->activeDocument == document)
        return;

    d->activeDocument = document;
    Q_EMIT activeDocumentChanged();
}

void SWindow::newDocument()
{
    d->containing << new SDocument(this);
    Q_EMIT documentsChanged();
}

void SWindow::closeDocument(int idx)
{
    auto taken = d->containing.takeAt(idx);
    Q_EMIT documentsChanged();
    taken->deleteLater();
}

void SWindow::transferDocumentTo(SDocument *document, SWindow *window)
{
    Q_ASSERT(d->containing.contains(document));

    window->d->containing << document;
    Q_EMIT window->documentsChanged();
    d->containing.removeAll(document);
    if (d->containing.isEmpty()) {
        d->containing << new SDocument(this);
    }
    Q_EMIT documentsChanged();
}
