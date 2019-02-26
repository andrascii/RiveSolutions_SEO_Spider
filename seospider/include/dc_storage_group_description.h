#pragma once

#include "storage_type.h"

namespace CrawlerEngine
{

class ICustomDataFeed;

}

namespace SeoSpider
{

enum class AuditGroup
{
	//
	// Problem groups
	//
	LinkAuditGroup,
	OnPageAuditGroup,
	H1AuditGroup,
	H2AuditGroup,
	ImageAuditGroup,
	PageProblemsAuditGroup,
	NotIndexedPagesGroup,

	//
	// Resources
	//
	InternalResourcesGroup,
	ExternalResourcesGroup,

	//
	// Problem type groups
	//
	Errors,
	Warnings,
	Information,
	NoProblem,

	//
	// Yandex Metrica Counters group
	//
	YandexMetricaCounters,

	//
	// Google Analytics Counters group
	//
	GoogleAnalyticsCounters,

	//
	// Custom data feeds
	//

	CustomDataFeeds,

	//
	// Temporary test group
	//
	OrderedErrorsGroup
};

struct DCStorageDescription
{
	CrawlerEngine::StorageType storageType;
	QString storageTypeDescriptionName;
	QString customDataFeed;
};

// DataCollection Storage Group Description
class DCStorageGroupDescription final : public QObject
{
	Q_OBJECT

public:
	~DCStorageGroupDescription();

	const QString& name() const noexcept;
	void setName(const QString& name);

	AuditGroup auditGroup() const noexcept;
	void setAuditGroup(AuditGroup auditGroup);

	const std::vector<DCStorageDescription>& descriptions() const noexcept;
	void setDescriptions(const std::vector<DCStorageDescription>& descriptions);
	void setDescriptions(std::vector<DCStorageDescription>&& descriptions);
	void addDescription(const DCStorageDescription& description);

	bool hasDescriptions() const noexcept;

signals:
	void changed();

private:
	QString m_name;
	AuditGroup m_auditGroup;
	std::vector<DCStorageDescription> m_descriptions;
};

using DCStorageGroupDescriptionPtr = Common::counted_ptr<DCStorageGroupDescription>;

template <typename... Args>
DCStorageGroupDescriptionPtr createDCStorageGroupDescriptionPtr(Args&&... args)
{
	return Common::make_counted<DCStorageGroupDescription>(std::forward<Args>(args)...);
}

}

Q_DECLARE_METATYPE(SeoSpider::DCStorageDescription)