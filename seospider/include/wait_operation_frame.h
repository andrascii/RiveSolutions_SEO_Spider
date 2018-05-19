#pragma once

#include "ui_wait_operation_frame.h"

namespace SeoSpider
{

class WaitOperationFrame final : public QFrame
{
public:
	static void show();
	static void finish();
	static void showMessage(const QString& message);

private:
	static WaitOperationFrame* instance();

	WaitOperationFrame(QWidget* parent);
	~WaitOperationFrame();

	void setMessage(const QString& message);

protected:
	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;

private:
	static QPointer<WaitOperationFrame> s_instance;

private:
	Ui_WaitOperationFrame* m_ui;
};

}