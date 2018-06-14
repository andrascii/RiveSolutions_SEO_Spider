#pragma once

#include "ui_site_map_creator_dialog.h"
#include "dialog.h"

namespace SeoSpider
{

class SitemapCreatorDialog : public Dialog
{
	Q_OBJECT

public:
	SitemapCreatorDialog(QWidget* parent = nullptr);

private slots:
	void showSaveFileDialog() const;
	void controlStateChanged() const;

private:
	std::unique_ptr<Ui_SitemapCreatorDialog> m_ui;
};

}