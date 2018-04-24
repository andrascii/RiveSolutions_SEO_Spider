#include "update_promoter.h"
#include "application.h"

namespace SeoSpider
{

UpdatePromoter::UpdatePromoter(QObject* parent) 
	: QObject(parent)
	, m_updaterProcess(new QProcess(this))
{
}

bool UpdatePromoter::updateAvailable()
{
	QVariant installPath = theApp->loadFromSettings("MetaData/InstallPath", QVariant());
	
	if(installPath == QVariant())
	{
		return false;
	}

	QString installPathString = installPath.toString().replace('\\', '/');
	QString programName = installPathString + "/Uninstall SeoSpider";
	QStringList argumentsList = { "--updater" };

	m_updaterProcess->setProgram(programName);
	m_updaterProcess->setArguments(argumentsList);
	m_updaterProcess->setWorkingDirectory(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first());
	
	m_updaterProcess->startDetached(programName, argumentsList);

	//m_updaterProcess->waitForFinished();
	//QString result(m_updaterProcess->readAllStandardOutput());

	//INFOLOG << m_updaterProcess->program();
	//INFOLOG << m_updaterProcess->workingDirectory();
	//INFOLOG << result;

	return false;
}

void UpdatePromoter::promoteUpdate()
{
}

}
