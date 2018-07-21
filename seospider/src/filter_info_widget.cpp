#include "filter_info_widget.h"
#include "application.h"
#include "preferences.h"

namespace SeoSpider
{

FilterInfoWidget::FilterInfoWidget(QWidget* parent)
	: QFrame(parent)
	, m_title(new QLabel(this))
	, m_description(new QLabel(this))
{
	m_title->setObjectName(QString("filterInfoWidgetTitle"));
	m_description->setObjectName(QString("filterInfoWidgetDescription"));

	QVBoxLayout* layout = new QVBoxLayout(this);
	setLayout(layout);
	layout->addWidget(m_title);
	layout->addWidget(m_description);

	m_description->setWordWrap(true);

	setObjectName(QString("filterInfo"));
}

void FilterInfoWidget::setFilterInfo(const FilterInfo& filterInfo)
{
	m_filterInfo = filterInfo;

	Preferences* preferences = theApp->preferences();
	const QMetaObject& meta = preferences->staticMetaObject;

	disconnect(preferences);

	foreach(const QByteArray& prop, filterInfo.props)
	{
		for (int i = 0; i < meta.propertyCount(); ++i)
		{
			const QMetaProperty& metaProperty = meta.property(i);
			QByteArray iPropName = metaProperty.name();
			if (iPropName == prop)
			{
				QByteArray signal = "2" + metaProperty.notifySignal().methodSignature();
				VERIFY(connect(preferences, signal, this, SLOT(onPropertyChanged())));
			}
		}
	}

	onPropertyChanged();
}

void FilterInfoWidget::onPropertyChanged()
{
	Preferences* preferences = theApp->preferences();

	m_title->setText(m_filterInfo.title);

	QString description = m_filterInfo.description;

	foreach(const QByteArray& prop, m_filterInfo.props)
	{
		description = description.arg(preferences->property(prop).toString());
	}

	m_description->setText(description);
}

}