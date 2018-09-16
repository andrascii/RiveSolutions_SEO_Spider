#include "dc_storage_group_description.h"

namespace SeoSpider
{

const QString& DCStorageGroupDescription::name() const noexcept
{
	return m_name;
}
void DCStorageGroupDescription::setName(const QString& name)
{
	m_name = name;

	emit changed();
}

AuditGroup DCStorageGroupDescription::auditGroup() const noexcept
{
	return m_auditGroup;
}
void DCStorageGroupDescription::setAuditGroup(AuditGroup auditGroup)
{
	m_auditGroup = auditGroup;

	emit changed();
}

const std::vector<DCStorageDescription>& DCStorageGroupDescription::descriptions() const noexcept
{
	return m_descriptions;
}
void DCStorageGroupDescription::setDescriptions(const std::vector<DCStorageDescription>& descriptions)
{
	m_descriptions = descriptions;

	emit changed();
}
void DCStorageGroupDescription::setDescriptions(std::vector<DCStorageDescription>&& descriptions)
{
	m_descriptions = std::move(descriptions);

	emit changed();
}
void DCStorageGroupDescription::addDescription(const DCStorageDescription& description)
{
	m_descriptions.push_back(description);

	emit changed();
}


bool DCStorageGroupDescription::hasDescriptions() const noexcept
{
	return !m_descriptions.empty();
}

}