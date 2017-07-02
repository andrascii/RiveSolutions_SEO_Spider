#pragma once

#include "website_analyse_element.h"

namespace QuickieWebBot
{

class ModelData
{
public:
	ModelData() = default;
	ModelData(QVector<WebsiteAnalyseElement> const * const dataStorage);

	void setDataStorage(QVector<WebsiteAnalyseElement> const * const dataStorage) noexcept;
	void setHeaderItems(std::initializer_list<QString> lst) noexcept;

protected:
	QVector<WebsiteAnalyseElement> const& dataStorage() const noexcept;
	QVector<QString> const& headerItems() const noexcept;

private:
	QVector<WebsiteAnalyseElement> const* m_dataStorage;
	QVector<QString> m_headerItems;
};

}