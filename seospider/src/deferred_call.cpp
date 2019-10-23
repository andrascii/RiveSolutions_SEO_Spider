#include "stdafx.h"
#include "deferred_call.h"

namespace SeoSpider
{
	
void DeferredCall::call(QPointer<QObject> object, const Function& function)
{
	DeferredCall* deferredCallEvent = new DeferredCall(object, function);

	QApplication::postEvent(DeferredCallProcessor::s_instance, deferredCallEvent);
}

void DeferredCall::call(const Function& function)
{
	DeferredCall* deferredCallEvent = new DeferredCall(function);

	QApplication::postEvent(DeferredCallProcessor::s_instance, deferredCallEvent);
}

DeferredCall::DeferredCall(QPointer<QObject> object, const Function& function)
	: QEvent(static_cast<QEvent::Type>(eventId()))
	, m_object(object)
	, m_needTargetObject(true)
	, m_function(function)
{
}

DeferredCall::DeferredCall(const Function& function)
	: QEvent(static_cast<QEvent::Type>(eventId()))
	, m_object(nullptr)
	, m_needTargetObject(false)
	, m_function(function)
{
}

void DeferredCall::execute()
{
	if (m_object || !m_needTargetObject)
	{
		m_function();
	}
}

int DeferredCall::eventId()
{
	static int s_id = QEvent::registerEventType();

	return s_id;
}

//////////////////////////////////////////////////////////////////////////

DeferredCallProcessor* DeferredCallProcessor::s_instance = nullptr;

void DeferredCallProcessor::init()
{
	DEBUG_ASSERT(!s_instance && "Attempt to create double DeferredCallProcessor objects");

	s_instance = new DeferredCallProcessor;
}

void DeferredCallProcessor::term()
{
	DEBUG_ASSERT(s_instance && "It seems that DeferredCallProcessor instance already destroyed or has not been instantiated");

	delete s_instance;

	s_instance = nullptr;
}

bool DeferredCallProcessor::event(QEvent* event)
{
	if (event->type() == DeferredCall::eventId())
	{
		DeferredCall* deferredCallEvent = static_cast<DeferredCall*>(event);

		deferredCallEvent->execute();

		return true;
	}

	return QObject::event(event);
}

}