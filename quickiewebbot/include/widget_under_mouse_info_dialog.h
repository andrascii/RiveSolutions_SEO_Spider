#pragma once

#include "ui_widget_under_mouse_info_dialog.h"

namespace QuickieWebBot
{
	
class WidgetUnderMouseInfoDialog : public QDialog
{
	Q_OBJECT

public:
	WidgetUnderMouseInfoDialog(QWidget* parent = nullptr);

	void showWithInfo(const QString& objectName, const QString& className);

protected:
	virtual void closeEvent(QCloseEvent* event) override;

private:
	std::unique_ptr<Ui_WidgetUnderMouseInfoDialog> m_ui;
};

}
