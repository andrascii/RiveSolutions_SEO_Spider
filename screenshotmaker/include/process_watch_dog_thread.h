#pragma once
#include "stdafx.h"

namespace ScreenshotMaker
{

class ProcessWatchDogThread : public QThread
{
	Q_OBJECT

public:
	ProcessWatchDogThread(const QString& processId, QObject* parent = nullptr);

	virtual void run() override;

signals:
	void processClosed();

private:
	HANDLE m_processHandle;
};

}