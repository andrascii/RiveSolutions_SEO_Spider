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
{
}

void UpdateChecker::check()
{
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

void UpdateChecker::onActualVersionFileLoaded(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadResponse& response)
{
	Q_UNUSED(requester);

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
	const Version actualVersion = stringToVersion(lastHop.body());
	const Version currentVersion = version(theApp->applicationFilePath());

	if (currentVersion == Version::invalidVersion)
	{
		//
		// TODO: implement sending report without shutdown program
		//
		ERRLOG << "ATTENTION!!! Cannot parse current program version!";
		return;
	}

	if (actualVersion.major <= currentVersion.major &&
		actualVersion.minor <= currentVersion.minor &&
		actualVersion.maintenance <= currentVersion.maintenance)
	{
		return;
	}

	CrawlerEngine::CrawlerRequest crawlerRequest
	{
		CrawlerEngine::Url(UpdateHelpers::downloadAddressFileUrl()),
		CrawlerEngine::DownloadRequestType::RequestTypeGet
	};

	CrawlerEngine::DownloadRequest request(crawlerRequest);
	m_downloadRequester.reset(request, this, &UpdateChecker::onDownloadLinkFileLoaded);
	m_downloadRequester->start();
}

void UpdateChecker::onDownloadLinkFileLoaded(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadResponse& response)
{
	Q_UNUSED(requester);

	m_downloadRequester.reset();

	ASSERT(response.hopsChain.length() != 0);

	const CrawlerEngine::Hop lastHop = response.hopsChain.back();

	emit updateExists(lastHop.body());
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

Version UpdateChecker::version(const QString& fileName) const
{
	DWORD dwHandle;
	DWORD dwLen = GetFileVersionInfoSizeW(fileName.toStdWString().c_str(), &dwHandle);

	std::unique_ptr<std::byte[]> lpData(new std::byte[dwLen]);

	if (!GetFileVersionInfoW(fileName.toStdWString().c_str(), dwHandle, dwLen, lpData.get()))
	{
		WARNLOG << "error in GetFileVersionInfo";
		return Version::invalidVersion;
	}

	VS_FIXEDFILEINFO *lpBuffer = NULL;
	UINT uLen;

	if (!VerQueryValueW(lpData.get(), QString("\\").toStdWString().c_str(), reinterpret_cast<LPVOID*>(&lpBuffer), &uLen))
	{
		WARNLOG << "error in VerQueryValue";
		return Version::invalidVersion;
	}

	return Version
	{
		(lpBuffer->dwFileVersionMS >> 16) & 0xffff,
		(lpBuffer->dwFileVersionMS) & 0xffff,
		(lpBuffer->dwFileVersionLS >> 16) & 0xffff
	};
}

}