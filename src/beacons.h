#pragma once

#include <QQuickItem>

class Beacon;

class BeaconControllerDelegate : public QObject
{
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("abstract interface")

public:
	explicit BeaconControllerDelegate(QObject* parent = nullptr);
	virtual ~BeaconControllerDelegate();

	virtual void dragEnterEvent(QDragEnterEvent*){};
	virtual void dragMoveEvent(QDragMoveEvent*){};
	virtual void dragLeaveEvent(QDragLeaveEvent*){};
	virtual void dropEvent(Beacon*, QDropEvent*){};
};

class BeaconController : public QQuickItem
{

	Q_OBJECT
	QML_NAMED_ELEMENT(BeaconController)

	Q_PROPERTY(QString activeBeaconID READ activeBeaconID NOTIFY activeBeaconIDChanged)
	Q_PROPERTY(BeaconControllerDelegate* delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)

	struct Private;
	QScopedPointer<Private> d;

	friend class Beacon;

	void addBeacon(Beacon* beacon);
	void delBeacon(Beacon* beacon);
	void computeActiveBeacon(const QPointF& point);

protected:
	void dragEnterEvent(QDragEnterEvent*) override;
	void dragMoveEvent(QDragMoveEvent*) override;
	void dragLeaveEvent(QDragLeaveEvent*) override;
	void dropEvent(QDropEvent*) override;

public:
	explicit BeaconController(QQuickItem* parent = nullptr);
	~BeaconController();

	BeaconControllerDelegate* delegate() const;
	void setDelegate(BeaconControllerDelegate* delegate);
	Q_SIGNAL void delegateChanged();

	QString activeBeaconID() const;
	Q_SIGNAL void activeBeaconIDChanged();
};

class Beacon : public QQuickItem
{

	Q_OBJECT
	QML_NAMED_ELEMENT(Beacon)

	Q_PROPERTY(BeaconController* controller READ controller WRITE setController NOTIFY controllerChanged REQUIRED)
	Q_PROPERTY(QString beaconID READ beaconID WRITE setBeaconID NOTIFY beaconIDChanged REQUIRED)
	Q_PROPERTY(QJSValue dataCallback READ dataCallback WRITE setDataCallback NOTIFY dataCallbackChanged REQUIRED)

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit Beacon(QQuickItem* parent = nullptr);
	~Beacon();

	BeaconController* controller() const;
	void setController(BeaconController* controller);
	Q_SIGNAL void controllerChanged();

	QString beaconID() const;
	void setBeaconID(const QString& beaconID);
	Q_SIGNAL void beaconIDChanged();

	QJSValue dataCallback() const;
	void setDataCallback(QJSValue dataCallback);
	Q_SIGNAL void dataCallbackChanged();

	QVariant data() const;
};