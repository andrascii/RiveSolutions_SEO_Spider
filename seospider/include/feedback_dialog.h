#pragma once

#include "ui_feedback_dialog.h"

namespace SeoSpider
{

class FeedbackDialog final : public QDialog
{
public:
	FeedbackDialog(QWidget* parent = nullptr);

private slots:
	void onSendFeedbackClicked();
	void onTextEditChanged();

private:
	Ui::FeedbackDialog* m_ui;
};

}