#pragma once
#include "stdafx.h"

namespace CrawlerEngine
{

class Session : public QObject
{
	Q_OBJECT

public:
	enum State
	{
		StateNone,
		StateSaved,
		StateUnsaved
	};

	Session(State state, const QString& name = QString(), QObject* parent = nullptr);

	State state() const noexcept;
	const QString& name() const noexcept;
	void setName(const QString& name);
	bool hasCustomName() const noexcept;

signals:
	void stateChanged(State state);
	void nameChanged(const QString& name);

public slots:
	void setState(State state);

private:
	State m_state;
	QString m_name;
	bool m_hasCustomName;
};

}