#include <QtMath>

#include "NGLib.h"
#include "beacons.h"

BeaconControllerDelegate::BeaconControllerDelegate(QObject* parent) : QObject(parent)
{
}

BeaconControllerDelegate::~BeaconControllerDelegate()
{
}

struct BeaconController::Private
{
	QString activeBeaconID;
	QPointer<Beacon> activeBeacon;
	QList<Beacon*> beacons;
	BeaconControllerDelegate* delegate;
};

BeaconController::BeaconController(QQuickItem* parent) : QQuickItem(parent), d(new Private)
{
	setFlag(QQuickItem::ItemAcceptsDrops, true);
}

BeaconController::~BeaconController()
{
}

void BeaconController::addBeacon(Beacon* beacon)
{
	d->beacons << beacon;
}

void BeaconController::delBeacon(Beacon* beacon)
{
	d->beacons.removeAll(beacon);
}

QString BeaconController::activeBeaconID() const
{
	return d->activeBeaconID;
}

inline qreal hypotenuse(qreal x, qreal y)
{
	return qSqrt(qPow(x, 2) + qPow(y, 2));
}

inline qreal distance(qreal x, qreal y, qreal xMin, qreal yMin, qreal xMax, qreal yMax)
{
	if (x < xMin) {
		if (y < yMin)
			return hypotenuse(xMin - x, yMin - y);
		else if (y <= yMax)
			return xMin - x;
		else
			return hypotenuse(xMin - x, yMax - y);
	} else if (x >= xMax) {
		if (y < yMin)
			return yMin - y;
		else if (y <= yMax)
			return 0;
		else
			return y - yMax;
	} else {
		if (y < yMin)
			return hypotenuse(xMax - x, yMin - y);
		else if (y <= yMax)
			return x - xMax;
		else
			return hypotenuse(xMax - x, yMax - y);
	}
}

inline qreal distance(const QPointF& point, const QRectF& rect)
{
	qreal xMin, yMin, xMax, yMax;
	rect.getCoords(&xMin, &yMin, &xMax, &yMax);

	return distance(point.x(), point.y(), xMin, yMin, xMax, yMax);
}

void BeaconController::computeActiveBeacon(const QPointF& point)
{
	if (d->beacons.size() == 0)
		return;

	using TheTuple = QPair<Beacon*, QRectF>;
	QList<TheTuple> beacons;
	beacons.reserve(d->beacons.size());

	for (auto* beacon : d->beacons) {
		beacons << qMakePair(beacon, beacon->mapRectToItem(this, beacon->boundingRect()));
	}

	std::sort(beacons.begin(), beacons.end(), [&point](const TheTuple& lhs, const TheTuple& rhs) {
		return distance(point, lhs.second) < distance(point, rhs.second);
	});
	const auto& theClosest = beacons.constFirst();

	if (theClosest.first->beaconID() == d->activeBeaconID)
		return;

	d->activeBeaconID = theClosest.first->beaconID();
	Q_EMIT activeBeaconIDChanged();
	d->activeBeacon = theClosest.first;
}

BeaconControllerDelegate* BeaconController::delegate() const
{
	return d->delegate;
}

void BeaconController::setDelegate(BeaconControllerDelegate* delegate)
{
	if (d->delegate == delegate)
		return;

	d->delegate = delegate;
	Q_EMIT delegateChanged();
}

void BeaconController::dragEnterEvent(QDragEnterEvent* event)
{
	computeActiveBeacon(event->posF());
	event->acceptProposedAction();

	if (d->delegate) d->delegate->dragEnterEvent(event);
}

void BeaconController::dragMoveEvent(QDragMoveEvent* event)
{
	computeActiveBeacon(event->posF());

	if (d->delegate) d->delegate->dragMoveEvent(event);
}

void BeaconController::dragLeaveEvent(QDragLeaveEvent* event)
{
	if (d->activeBeaconID.isEmpty())
		return;

	d->activeBeaconID = QString();
	Q_EMIT activeBeaconIDChanged();

	if (d->delegate) d->delegate->dragLeaveEvent(event);
}

void BeaconController::dropEvent(QDropEvent* event)
{
	if (d->activeBeaconID.isEmpty())
		return;

	d->activeBeaconID = QString();
	Q_EMIT activeBeaconIDChanged();

	if (d->delegate) d->delegate->dropEvent(d->activeBeacon, event);
}

struct Beacon::Private
{
	QPointer<BeaconController> controller = nullptr;
	QString beaconID;
	QJSValue dataCallback;
};

Beacon::Beacon(QQuickItem* parent) : QQuickItem(parent), d(new Private)
{
}

Beacon::~Beacon()
{
	if (d->controller) d->controller->delBeacon(this);
}

BeaconController* Beacon::controller() const
{
	return d->controller;
}

void Beacon::setController(BeaconController* controller)
{
	if (d->controller == controller)
		return;

	if (d->controller != nullptr) {
		d->controller->delBeacon(this);
	}
	d->controller = controller;
	d->controller->addBeacon(this);
	Q_EMIT controllerChanged();
}

QString Beacon::beaconID() const
{
	return d->beaconID;
}

void Beacon::setBeaconID(const QString& beaconID)
{
	if (d->beaconID == beaconID)
		return;

	d->beaconID = beaconID;
	Q_EMIT beaconIDChanged();
}

QJSValue Beacon::dataCallback() const
{
	return d->dataCallback;
}

void Beacon::setDataCallback(QJSValue dataCallback)
{
	d->dataCallback = dataCallback;
	Q_EMIT dataCallbackChanged();
}

QVariant Beacon::data() const
{
	return d->dataCallback.call().toVariant();
}
