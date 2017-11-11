#include "site_map_creator_widget.h"

namespace SeoSpider
{

SitemapCreatorWidget::SitemapCreatorWidget(QWidget* parent)
	: QDialog(parent)
	, m_ui(new Ui_SitemapCreatorWidget)
{
	m_ui->setupUi(this);

	setWindowTitle("XML Sitemap Creator");
}

}