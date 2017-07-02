#pragma once

#include "website_analyse_element.h"

namespace QuickieWebBot
{

class ModelData
{
public:
	ModelData() = default;
	ModelData(std::vector<WebsiteAnalyseElement> const * const dataStorage);

	void setDataStorage(std::vector<WebsiteAnalyseElement> const * const dataStorage) noexcept;
	void setHeaderItems(std::initializer_list<QString> lst) noexcept;

protected:
	std::vector<WebsiteAnalyseElement> const& dataStorage() const noexcept;
	std::vector<QString> const& headerItems() const noexcept;

private:
	std::vector<WebsiteAnalyseElement> const* m_dataStorage;
	std::vector<QString> m_headerItems;
};

}