#include "update_checker.h"
#include "download_request.h"
#include "download_response.h"
#include "crawler_request.h"
#include "application.h"
#include "update_helpers.h"

#ifdef Q_OS_WIN
#pragma comment(lib, "mincore.lib")
#endif

namespace SeoSpider
{

const Version Version::invalidVersion{ -1, -1, -1 };

UpdateChecker::UpdateChecker(QObject* parent)
	: QObject(parent)
	, m_thisProgramVersion(getLocalVersion())
{
}

void UpdateChecker::check()
{
	INFOLOG << "Start actual_version.txt loading";

	CrawlerEngine::CrawlerRequest crawlerRequest
	{
		CrawlerEngine::Url(UpdateHelpers::actualVersionFileUrl()),
		CrawlerEngine::DownloadRequestType::RequestTypeGet
	};

	CrawlerEngine::DownloadRequest request(crawlerRequest);
	m_downloadRequester.reset(request, this, &UpdateChecker::onActualVersionFileLoaded);
	m_downloadRequester->start();
}

QObject* UpdateChecker::qobject() const noexcept
{
	return const_cast<UpdateChecker*>(this);
}

Version UpdateChecker::getVerstionStr()
{
	return m_thisProgramVersion;
}

void UpdateChecker::onActualVersionFileLoaded(CrawlerEngine::Requester*, const CrawlerEngine::DownloadResponse& response)
{
	INFOLOG << "actual_version.txt loaded";

	m_downloadRequester.reset();

	ASSERT(response.hopsChain.length() != 0);

	const CrawlerEngine::Hop lastHop = response.hopsChain.back();

	if (lastHop.statusCode() != Common::StatusCode::Ok200)
	{
		ERRLOG << "ATTENTION!!! Cannot load actual program version!";
		return;
	}

	//
	// TODO: implement decryption when encryption will be implemented on the server
	//
	const Version actualVersion = getRemoteVersion(lastHop.body());

	if(m_thisProgramVersion < actualVersion)
	{
		emit updateExists();
	}
}

Version UpdateChecker::stringToVersion(const QString& versionString) const
{
	const QRegularExpression versionStringRegExp("^\\d\\.\\d\\.\\d$");

	const QRegularExpressionMatch match = versionStringRegExp.match(versionString);
	const bool hasMatch = match.hasMatch();

	ASSERT(hasMatch);

	const QStringList matchParts = versionString.split(".");

	return Version
	{
		matchParts[0].toInt(),
		matchParts[1].toInt(),
		matchParts[2].toInt()
	};
}

Version UpdateChecker::getLocalVersion() const
{
	QFile* componentsFile = new QFile(UpdateHelpers::localVersionXmlFile());

	if (!componentsFile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return Version::invalidVersion;
	}

	QXmlStreamReader componentsXmlFile(componentsFile);

	while (!componentsXmlFile.atEnd() && !componentsXmlFile.hasError())
	{
		QXmlStreamReader::TokenType token = componentsXmlFile.readNext();
		if (token == QXmlStreamReader::StartDocument)
			continue;
		if (token == QXmlStreamReader::StartElement)
		{
			if (componentsXmlFile.name() == "Package")
			{
				while (!(componentsXmlFile.tokenType() == QXmlStreamReader::EndElement && componentsXmlFile.name() == "PackageUpdate"))
				{
					if (componentsXmlFile.tokenType() == QXmlStreamReader::StartElement)
					{
						if (componentsXmlFile.name() == "Version")
						{
							if (componentsXmlFile.tokenType() == QXmlStreamReader::StartElement)
							{
								componentsXmlFile.readNext();
								const Version actualVersion = stringToVersion(componentsXmlFile.text().toString());
								return actualVersion;
							}
						}
					}

					componentsXmlFile.readNext();
				}
			}
		}
	}

	return Version::invalidVersion;
}

Version UpdateChecker::getRemoteVersion(const QByteArray& remoteXmlUpdateFile) const
{
	QXmlStreamReader updateFile(remoteXmlUpdateFile);

	while (!updateFile.atEnd() && !updateFile.hasError())
	{
		QXmlStreamReader::TokenType token = updateFile.readNext();
		if (token == QXmlStreamReader::StartDocument)
			continue;
		if (token == QXmlStreamReader::StartElement)
		{
			if (updateFile.name() == "PackageUpdate")
			{
				while (!(updateFile.tokenType() == QXmlStreamReader::EndElement && updateFile.name() == "PackageUpdate"))
				{
					if (updateFile.tokenType() == QXmlStreamReader::StartElement)
					{
						if (updateFile.name() == "Version")
						{
							if (updateFile.tokenType() == QXmlStreamReader::StartElement)
							{
								updateFile.readNext();
								const Version actualVersion = stringToVersion(updateFile.text().toString());
								return actualVersion;
							}
						}
					}

					updateFile.readNext();
				}
			}
		}
	}

	return Version::invalidVersion;
}

}