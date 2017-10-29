#include "move_to_thread_service.h"
#include "named_thread.h"

namespace SeoSpider
{

MoveToThreadService::~MoveToThreadService()
{
	foreach(const ObjectsThreadAffinity& objectsThreadAffinity, m_objectsThreadAffinity)
	{
		//
		// delete objects in another thread
		// and wait their destruction
		//
		deleteAllLater(objectsThreadAffinity.objects);
		qApp->processEvents();

		objectsThreadAffinity.thread->quit();
		objectsThreadAffinity.thread->wait();

		delete objectsThreadAffinity.thread;
	}
}

void MoveToThreadService::moveObjectToThread(QObject* object, const QByteArray& threadName)
{
	if (m_objectsThreadAffinity.contains(threadName))
	{
		if (m_objectsThreadAffinity[threadName].objects.contains(object))
		{
			return;
		}

		object->moveToThread(m_objectsThreadAffinity[threadName].thread);
		return;
	}

	m_objectsThreadAffinity[threadName].thread = new Common::NamedThread(threadName);
	object->moveToThread(m_objectsThreadAffinity[threadName].thread);
	m_objectsThreadAffinity[threadName].objects.append(object);
	m_objectsThreadAffinity[threadName].thread->start();
}

void MoveToThreadService::deleteAllLater(const QList<QObject*>& objects)
{
	foreach(QObject* qobject, objects)
	{
		qobject->deleteLater();
	}
}

}