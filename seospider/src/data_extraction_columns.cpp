#include "data_extraction_columns.h"
#include "application.h"
#include "preferences.h"
#include "helpers.h"

namespace SeoSpider
{

DataExtractionColumns::DataExtractionColumns()
{
	const QMetaMethod thisSlotMetaMethod = Common::Helpers::metaMethodOfSlot(this, "onSomeExtractorRuleTypeChanged()");

	for (size_t i = 0; i < Preferences::c_extractorCount; ++i)
	{
		const QByteArray propertyName = QStringLiteral("extractorRuleType%1").arg(i + 1).toLatin1();
		const QMetaMethod signal = Common::Helpers::metaMethodOfSignal(theApp->preferences(), propertyName + "Changed(int)");
		Common::Helpers::connectMetaMethods(theApp->preferences(), signal, this, thisSlotMetaMethod);
	}

	onSomeExtractorRuleTypeChanged();
}

QString DataExtractionColumns::data(const CrawlerEngine::ParsedPage* page, int columnIndex) const noexcept
{
	Q_UNUSED(page);
	Q_UNUSED(columnIndex);

	return page->dataExtractions.value(columnIndex, QString());
}

int DataExtractionColumns::columnCount() const noexcept
{
	return m_columnNames.size();
}

const QString& DataExtractionColumns::columnDescription(int columnIndex) const noexcept
{
	ASSERT(columnIndex < columnCount());
	return m_columnNames[columnIndex];
}

int DataExtractionColumns::columnWidth(int columnIndex) const noexcept
{
	Q_UNUSED(columnIndex);
	// TODO: use more smart solution to determine the current QHeaderView::section font
	//QFontMetrics fontMetrics(QFont("Arial", 8, QFont::Normal));
	//return fontMetrics.horizontalAdvance(m_columnNames[columnIndex]);
	return 100;
}

QObject* DataExtractionColumns::qobject() const
{
	return const_cast<DataExtractionColumns*>(this);
}

void DataExtractionColumns::onSomeExtractorRuleTypeChanged()
{
	m_columnNames.clear();

	for (size_t i = 0; i < Preferences::c_extractorCount; ++i)
	{
		const QByteArray extractorRuleTypePropertyName = QStringLiteral("extractorRuleType%1").arg(i + 1).toLatin1();
		const bool extractorIsDisabled = theApp->preferences()->property(extractorRuleTypePropertyName.constData()).toInt() == -1;

		if (extractorIsDisabled)
		{
			continue;
		}

		const QByteArray extractorNamePropertyName = QStringLiteral("extractorName%1").arg(i + 1).toLatin1();
		const QString& extractorName = theApp->preferences()->property(extractorNamePropertyName.constData()).toString();
		m_columnNames.push_back(extractorName.isEmpty() ? tr("Extractor %1").arg(i + 1) : extractorName);
	}

	emit columnCountChanged(m_columnNames.size());
}

}