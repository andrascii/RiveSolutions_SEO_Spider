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

Version version(const QString& fileName)
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

const Version Version::invalidVersion{ -1, -1, -1 };

UpdateChecker::UpdateChecker(QObject* parent)
	: QObject(parent)
	, m_thisProgramVersion(version(theApp->applicationFilePath()))
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
	const Version currentVersion = version(theApp->applicationFilePath());

	if(currentVersion == Version::invalidVersion)
	{
		QFile::remove(updatePatchInfo.second);

		theApp->removeKeyFromSettings(UpdateHelpers::updatePatchSavePathKey());

		return;
	}

	if (updatePatchInfo.first)
	{
		ASSERT(!updatePatchInfo.second.isEmpty());

		emit updateAlreadyDownloaded(updatePatchInfo.second);

		return;
	}
	else
	{
		theApp->removeKeyFromSettings(UpdateHelpers::updatePatchSavePathKey());
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

void UpdateChecker::onDownloadLinkFileLoaded(CrawlerEngine::Requester*, const CrawlerEngine::DownloadResponse& response)
{
	INFOLOG << "download_address.txt loaded";

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

Version UpdateChecker::getRemoteVersion(const QByteArray& remoteXmlUpdateFile)
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

Version UpdateChecker::version(const QString& fileName) const
{
	DWORD dwHandle;
	DWORD dwLen = GetFileVersionInfoSizeW(fileName.toStdWString().c_str(), &dwHandle);

	return true;
}

std::pair<bool, QString> UpdateChecker::checkExistenceUpdatePatch() const
{
	const QVariant saveUpdatePathKey = theApp->loadFromSettings(UpdateHelpers::updatePatchSavePathKey());

	QString downloadedFilePath;
	bool updatePatchAlreadyExists = false;

	if (saveUpdatePathKey.isValid())
	{
		downloadedFilePath = saveUpdatePathKey.toString();

		if (QFile::exists(downloadedFilePath))
		{
			updatePatchAlreadyExists = true;
		}
	}

	return std::pair(updatePatchAlreadyExists, downloadedFilePath);
}

}