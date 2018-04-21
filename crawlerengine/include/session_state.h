#pragma once

namespace CrawlerEngine
{

class SessionState : public QObject
{
	Q_OBJECT

public:
	enum State
	{
		StateSaved,
		StateUnsaved
	};

	SessionState(State state, QObject* parent = nullptr);

signals:
	void stateChanged(int state);

public slots:
	void setState(int state);

private:
	State m_state;
};

}