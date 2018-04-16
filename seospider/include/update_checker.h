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

	friend bool operator==(const Version& lhs, const Version& rhs)
	{
		return lhs.major == rhs.major && 
			lhs.minor == rhs.minor && 
			lhs.maintenance == rhs.maintenance;
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
	virtual void updateExists(const QString& downloadLink) = 0;
};

class UpdateChecker : public QObject, public IUpdateChecker
{
	Q_OBJECT

public:
	UpdateChecker(QObject* parent = nullptr);

	virtual void check() override;
	virtual QObject* qobject() const noexcept override;

signals:
	virtual void updateExists(const QString& downloadLink) override;

private:
	void onActualVersionFileLoaded(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadResponse& response);
	void onDownloadLinkFileLoaded(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadResponse& response);
	Version stringToVersion(const QString& versionString) const;

private:
	Version version(const QString& fileName) const;

private:
	CrawlerEngine::RequesterWrapper m_downloadRequester;
};

}