#pragma once

namespace SeoSpider
{

class IMoveToThreadService
{
public:
	virtual ~IMoveToThreadService() = default;
	virtual void moveObjectToThread(QObject* object, const QByteArray& threadName) = 0;
};

class MoveToThreadService : public IMoveToThreadService
{
public:
	virtual ~MoveToThreadService();

	virtual void moveObjectToThread(QObject* object, const QByteArray& threadName) override;

private:
	static void deleteAllLater(const QList<QObject*>& objects);

private:
	struct ObjectsThreadAffinity
	{
		QList<QObject*> objects;
		QThread* thread;
	};

	QMap<QByteArray, ObjectsThreadAffinity> m_objectsThreadAffinity;
};

}
