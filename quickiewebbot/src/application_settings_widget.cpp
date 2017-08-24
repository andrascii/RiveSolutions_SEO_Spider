#include "application_settings_widget.h"

namespace QuickieWebBot
{

ApplicationSettingsWidget::ApplicationSettingsWidget(QWidget* parent)
	: QWidget(parent, Qt::Dialog)
{
	initialize();
}

void ApplicationSettingsWidget::initialize()
{
	m_ui.setupUi(this);
		
	ApplicationSettingsModel* model = new ApplicationSettingsModel;
	m_ui.propGroupsList->setModel(model);

	QHBoxLayout* layout = new QHBoxLayout(this);
	QWidget* c = new CrawlerSettingsWidget(this);
	layout->addWidget(c);
	layout->setAlignment(Qt::AlignTop);
	m_ui.currentGroup->setLayout(layout);
	
}

}