#pragma once

namespace SeoSpider
{
	
class DeferredCall : public QEvent
{
public:
	using Function = std::function<void()>;

	static void call(QPointer<QObject> object, const Function& function);

	static void call(const Function& function);

private:
	friend class DeferredCallProcessor;

	DeferredCall(QPointer<QObject> object, const Function& function);

	DeferredCall(const Function& function);

	void execute();

	static int eventId();

private:
	QPointer<QObject> m_object;

	bool m_needTargetObject;

	Function m_function;
};

class DeferredCallProcessor : public QObject
{
public:
	static void init();

	static void term();

protected:
	virtual bool event(QEvent* event) override;

private:
	static DeferredCallProcessor* s_instance;

	friend DeferredCall;
};

}