#include "service.h"

SPService::SPService(KService::Ptr ptr) : m_service(ptr)
{
	static bool m_registered = false;
	if (!m_registered) {
		qRegisterMetaType<QList<SPService>>();
	}
}

SPService::SPService() : SPService(KService::Ptr())
{
}

SPService::~SPService()
{
}