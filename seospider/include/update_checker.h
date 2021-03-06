#pragma once

#include "requester_wrapper.h"

namespace CrawlerEngine
{

struct DownloadResponse;

}

namespace SeoSpider
{

struct Version
{
	int major;
	int minor;
	int maintenance;

	friend bool operator<(const Version& lhs, const Version& rhs)
	{
		return lhs.major < rhs.major ||
			(lhs.major <= rhs.major && lhs.minor < rhs.minor) ||
			(lhs.major <= rhs.major && lhs.minor <= rhs.minor && lhs.maintenance < rhs.maintenance);
	}

	friend bool operator>(const Version& lhs, const Version& rhs)
	{
		return !(lhs < rhs) && lhs != rhs;
	}

	friend bool operator<=(const Version& lhs, const Version& rhs)
	{
		return lhs < rhs || lhs == rhs;
	}

	friend bool operator>=(const Version& lhs, const Version& rhs)
	{
		return lhs > rhs || lhs == rhs;
	}

	friend bool operator==(const Version& lhs, const Version& rhs)
	{
		return !(lhs < rhs) && !(rhs < lhs);
	}

	friend bool operator!=(const Version& lhs, const Version& rhs)
	{
		return !(lhs == rhs);
	}

	static const Version invalidVersion;
};

class IUpdateChecker
{
public:
	virtual ~IUpdateChecker() = default;
	virtual void check() = 0;
	virtual QObject* qobject() const noexcept = 0;

	// signals
	virtual void updateExists() = 0;
	virtual void updateIsNotExists() = 0;
};

class UpdateChecker : public QObject, public IUpdateChecker
{
	Q_OBJECT

public:
	UpdateChecker(QObject* parent = nullptr);

	virtual void check() override;
	virtual QObject* qobject() const noexcept override;
	Version getVerstionStr();

signals:
	virtual void updateExists() override;
	virtual void updateIsNotExists() override;

private:
	void onActualVersionFileLoaded(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadResponse& response);
	Version stringToVersion(const QString& versionString) const;
	Version getRemoteVersion(const QByteArray& remoteXmlUpdateFile) const;
	Version getLocalVersion() const;

private:
	CrawlerEngine::RequesterWrapper m_downloadRequester;
	Version m_thisProgramVersion;
};

}

Q_DECLARE_METATYPE(SeoSpider::Version)
