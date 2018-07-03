#include "update_checker.h"
#include "download_request.h"
#include "download_response.h"
#include "crawler_request.h"
#include "application.h"
#include "update_helpers.h"
#include "version.h"

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
	INFOLOG << "Start" << UpdateHelpers::actualVersionFileUrl() << "loading";

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
	INFOLOG << UpdateHelpers::actualVersionFileUrl() << "loaded";

	m_downloadRequester.reset();

	ASSERT(response.hopsChain.length() != 0);

	const CrawlerEngine::Hop lastHop = response.hopsChain.lastHop();

	if (lastHop.statusCode() != Common::StatusCode::Ok200)
	{
		ERRLOG << "ATTENTION!!! Cannot load actual program version!";

		emit updateIsNotExists();

		return;
	}

	const Version actualVersion = getRemoteVersion(lastHop.body());

	if(m_thisProgramVersion < actualVersion)
	{
		emit updateExists();
	}
	else
	{
		emit updateIsNotExists();
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
	return Version{ MAJOR, MINOR, MAINTENANCE };
}

Version UpdateChecker::getRemoteVersion(const QByteArray& remoteXmlUpdateFile) const
{
	QXmlStreamReader updateFile(remoteXmlUpdateFile);

	while (!updateFile.atEnd() && !updateFile.hasError())
	{
		QXmlStreamReader::TokenType token = updateFile.readNext();

		if (token == QXmlStreamReader::StartDocument)
		{
			continue;
		}

		if (token != QXmlStreamReader::StartElement)
		{
			continue;
		}

		if (updateFile.name() != "PackageUpdate")
		{
			continue;
		}

		while (!(updateFile.tokenType() == QXmlStreamReader::EndElement && updateFile.name() == "PackageUpdate"))
		{
			if (updateFile.tokenType() == QXmlStreamReader::StartElement && 
				updateFile.name() == "Version" && 
				updateFile.tokenType() == QXmlStreamReader::StartElement)
			{
				updateFile.readNext();
				const Version actualVersion = stringToVersion(updateFile.text().toString());

				return actualVersion;
			}

			updateFile.readNext();
		}
	}

	return Version::invalidVersion;
}

}