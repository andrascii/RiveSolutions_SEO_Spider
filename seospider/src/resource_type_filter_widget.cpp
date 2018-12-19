#include "resource_type_filter_widget.h"
#include "row_resource_type.h"

namespace SeoSpider
{

ResourceTypeFilterWidget::ResourceTypeFilterWidget(QWidget* parent)
	: QFrame(parent)
	, m_filter(ResourceAny)
{
	setObjectName("resourceTypeFilter");
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addStretch();
	createPushButton(QObject::tr("HTML"), ResourceHtml, layout, "left");
	createPushButton(QObject::tr("JS"), ResourceJavaScript, layout);
	createPushButton(QObject::tr("CSS"), ResourceStyleSheet, layout);
	createPushButton(QObject::tr("Images"), ResourceImage, layout);
	// createPushButton(QObject::tr("Flash"), ResourceFlash, layout);
	// createPushButton(QObject::tr("Video"), ResourceVideo, layout);
	createPushButton(QObject::tr("Other"), ResourceFlash | ResourceVideo | ResourceOther, layout, "right");

	setLayout(layout);
}

void ResourceTypeFilterWidget::setFilter(int filter)
{
	foreach (int key, m_buttons.keys())
	{
		const bool filterEnabled = key & filter;
		m_buttons[key]->setChecked(filterEnabled);
	}
}

void ResourceTypeFilterWidget::createPushButton(const QString& text, int flag, QHBoxLayout* layout, const char* prop)
{
	QPushButton* button = new QPushButton(this);
	button->setObjectName("filterButton");
	button->setCheckable(true);
	button->setChecked(true);
	button->setText(text);
	layout->addWidget(button);
	button->setContentsMargins(0, 0, 0, 0);

	m_buttons[flag] = button;

	if (prop != nullptr)
	{
		button->setProperty(prop, true);
	}

	connect(button, &QPushButton::clicked, this, [this, button, flag] {
		if (button->isChecked())
		{
			m_filter |= flag;
		}
		else
		{
			m_filter &= ~flag;
		}

		emit filterChanged(m_filter);
	});
}
}
