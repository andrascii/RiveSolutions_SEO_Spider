#pragma once

namespace SeoSpider
{

class UpdatePromoter : public QObject
{
	Q_OBJECT

public:
	UpdatePromoter(QObject* parent = nullptr);
	
	bool updateAvailable();
	void promoteUpdate();

private:
	QProcess* m_updaterProcess;
};

}