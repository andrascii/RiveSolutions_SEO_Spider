#pragma once
#include "stdafx.h"

#include "ui_feedback_dialog.h"
#include "dialog.h"

namespace SeoSpider
{

class FeedbackDialog final : public Dialog
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