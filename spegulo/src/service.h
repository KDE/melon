#pragma once

#include <QtQml>
#include <KService>

struct SPService
{
	Q_GADGET

	QML_NAMED_ELEMENT(Service)
	QML_UNCREATABLE("must be gotten from c++")

	KService::Ptr m_service;

public:
	explicit SPService(KService::Ptr ptr);
	explicit SPService();
	~SPService();

	operator KService::Ptr()
	{
		return m_service;
	}

	Q_PROPERTY(bool isNull READ isNull)
	bool isNull() const { return m_service == nullptr; }

	Q_PROPERTY(QString icon READ icon)
	QString icon() const
	{
		if (isNull())
			return {};
		return m_service->icon();
	}

	Q_PROPERTY(QString name READ name)
	QString name() const
	{
		if (isNull())
			return {};
		return m_service->name();
	}
};

Q_DECLARE_METATYPE(SPService)
Q_DECLARE_METATYPE(QList<SPService>)
