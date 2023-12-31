#include <KIO/PreviewJob>
#include <QQuickItem>
#include <QQuickWindow>

#include "previewer.h"

struct SPreviewer::Private
{
	SPreviewer* p;
	QPixmap pixmap;
	KFileItem fileItem;
	QQuickItem* quickItem = nullptr;
	KIO::PreviewJob* job = nullptr;
	QSize size;
	bool complete = false;
	bool enabled = true;

	void makePixmap();
};

SPreviewer::SPreviewer(QObject* parent) : QObject(parent), d(new Private)
{
	d->p = this;
}

SPreviewer::~SPreviewer()
{
}

bool SPreviewer::enabled() const
{
	return d->enabled;
}

void SPreviewer::setEnabled(bool enabled)
{
	if (enabled == d->enabled)
		return;

	d->enabled = enabled;
	Q_EMIT enabledChanged();

	if (!d->enabled) {
		d->pixmap = QPixmap();
		Q_EMIT pixmapChanged();
	}
}

QPixmap SPreviewer::pixmap() const
{
	return d->pixmap;
}

QSize SPreviewer::size() const
{
	return d->size;
}

void SPreviewer::Private::makePixmap()
{
	if (!complete || !enabled)
		return;

	if (job) {
		job->deleteLater();
	}

	job = new KIO::PreviewJob(KFileItemList{fileItem}, size);
	if (quickItem && quickItem->window()) job->setDevicePixelRatio(quickItem->window()->effectiveDevicePixelRatio());

	QObject::connect(job, &KIO::PreviewJob::failed, p, [this](const KFileItem& item) {
		Q_UNUSED(item)

		pixmap = QPixmap();
		Q_EMIT p->pixmapChanged();

		if (job) {
			job->deleteLater();
			job = nullptr;
		}
	});
	QObject::connect(job, &KIO::PreviewJob::gotPreview, p, [this](const KFileItem& item, const QPixmap& preview) {
		Q_UNUSED(item)

		pixmap = preview;
		Q_EMIT p->pixmapChanged();

		if (job) {
			job->deleteLater();
			job = nullptr;
		}
	});
}

void SPreviewer::setSize(const QSize& size)
{
	if (d->size == size)
		return;

	d->size = size;
	Q_EMIT sizeChanged();

	d->makePixmap();
}

void SPreviewer::componentComplete()
{
	d->complete = true;
	d->makePixmap();
}

KFileItem SPreviewer::fileItem() const
{
	return d->fileItem;
}

void SPreviewer::setFileItem(KFileItem item)
{
	if (d->fileItem == item)
		return;

	d->fileItem = item;
	Q_EMIT fileItemChanged();

	d->makePixmap();
}

QQuickItem* SPreviewer::item() const
{
	return d->quickItem;
}

void SPreviewer::setItem(QQuickItem* item)
{
	if (item == d->quickItem)
		return;

	d->quickItem = item;
	Q_EMIT itemChanged();

	d->makePixmap();
}
