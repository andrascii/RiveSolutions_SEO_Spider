#pragma once
#include "stdafx.h"
#ifndef ZIPPO_H
#define ZIPPO_H

#include <cstddef>

class Zippo
	: public QThread
{
Q_OBJECT
	
public:
	
	Zippo();
	~Zippo();
	
	void zcompress(const QDir& reportDir, const QString& reportfile, const QStringList& userFiles, const QString& description);
	void cancel();

	QString errorString() const { return m_error; }

signals:
	
	void finished();
	void error();

	void startCompression(const QDir& reportDir, const QString& reportfile, const QStringList& userFiles, const QString& description);

protected slots:

	void onStartCompression(const QDir& reportDir, const QString& reportfile, const QStringList& userFiles, const QString& description);

private:

	volatile bool m_isCancelling;
	QString m_error;
};

#endif // ZIPPO_H
