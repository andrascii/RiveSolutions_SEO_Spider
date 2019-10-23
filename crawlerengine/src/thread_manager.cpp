#include "stdafx.h"
#include "thread_manager.h"
#include "named_thread.h"

namespace
{

CrawlerEngine::ThreadManager* s_threadManager = nullptr;

}

namespace CrawlerEngine
{

ThreadManager::~ThreadManager()
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

void ThreadManager::moveObjectToThread(QObject* object, const QByteArray& threadName)
{
	if (m_objectsThreadAffinity.contains(threadName))
	{
		if (m_objectsThreadAffinity[threadName].objects.contains(object))
		{
			return;
		}

		object->moveToThread(m_objectsThreadAffinity[threadName].thread);
		m_objectsThreadAffinity[threadName].objects.append(object);
		return;
	}

	m_objectsThreadAffinity[threadName].thread = new Common::NamedThread(threadName);
	object->moveToThread(m_objectsThreadAffinity[threadName].thread);
	m_objectsThreadAffinity[threadName].objects.append(object);
	m_objectsThreadAffinity[threadName].thread->start();
}

void ThreadManager::deleteAllLater(const QList<QObject*>& objects)
{
	foreach(QObject* qobject, objects)
	{
		qobject->deleteLater();
	}
}

CrawlerEngine::ThreadManager& ThreadManager::instance()
{
	if (!s_threadManager)
	{
		s_threadManager = new ThreadManager;
	}

	return *s_threadManager;
}

void ThreadManager::destroy()
{
	delete s_threadManager;

	s_threadManager = nullptr;
}

}