#include "storage_exporter.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"
#include "xlsxrichstring.h"
#include "isequenced_storage.h"
#include "sequenced_data_collection.h"
#include "parsed_page_info.h"
#include "storage_adapter_factory.h"
#include "application.h"
#include "main_window.h"
#include "service_locator.h"
#include "inotification_service.h"

namespace SeoSpider
{

void StorageExporter::exportStorage(const CrawlerEngine::SequencedDataCollection* dataCollection, 
	const std::vector<DCStorageDescription>& storages)
{
	ServiceLocator* serviceLocator = ServiceLocator::instance();

	const QString filepath = QFileDialog::getSaveFileName(
		theApp->mainWindow(),
		QObject::tr("Save To Excel"),
		QString(),
		QString("*.xlsx")
	);

	if (filepath.isEmpty())
	{
		serviceLocator->service<INotificationService>()->error(
			QObject::tr("Storage Exporter"),
			QObject::tr("Invalid path. Data were not exported.")
		);

		return;
	}

	QXlsx::Document xlsxDocument(filepath);

	int rowNumber = 1;

	QXlsx::Format headerFormat;
	headerFormat.setFontColor(Qt::red);
	headerFormat.setFontSize(13);

	QXlsx::Format columnFormat;
	columnFormat.setFontColor(Qt::red);
	columnFormat.setFontSize(13);

	for (int i = 0, sz = static_cast<int>(storages.size()); i < sz; ++i)
	{
		const CrawlerEngine::StorageType storageType = storages[i].storageType;
		const CrawlerEngine::ISequencedStorage& sequencedStorage = *dataCollection->storage(storageType);
		const int storageSize = sequencedStorage.size();

		QXlsx::RichString headerRichString;
		headerRichString.addFragment(storages[i].storageTypeDescriptionName + QStringLiteral(" (%1 items)").arg(storageSize), headerFormat);

		xlsxDocument.write(rowNumber++, 1, headerRichString);

		QVector<ParsedPageInfo::Column> columnsForType =
			StorageAdapterFactory::parsedPageAvailableColumns(static_cast<StorageAdapterType>(storageType));

		for (int k = 1; k <= columnsForType.size(); ++k)
		{
			QXlsx::RichString columnRichString;
			columnRichString.addFragment(ParsedPageInfo::itemTypeDescription(columnsForType[k - 1]), columnFormat);

			xlsxDocument.write(rowNumber, k, columnRichString);
		}

		++rowNumber;

		for (int j = 0; j < storageSize; ++j, ++rowNumber)
		{
			ParsedPageInfo parsedPageInfoProvider(sequencedStorage[j]);

			for (int k = 1; k <= columnsForType.size(); ++k)
			{
				xlsxDocument.write(rowNumber, k, parsedPageInfoProvider.itemValue(columnsForType[k - 1]));
			}
		}

		++rowNumber;
	}

	xlsxDocument.save();

	serviceLocator->service<INotificationService>()->info(
		QObject::tr("Storage Exporter"),
		QObject::tr("Data by specified filters were successful exported to the\n%1 file.").arg(filepath)
	);
}

}