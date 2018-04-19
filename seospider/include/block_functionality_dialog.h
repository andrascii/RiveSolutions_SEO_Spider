#pragma once

#include "ui_block_functionality_dialog.h"

class BlockFunctionalityDialog : public QDialog
{
	Q_OBJECT

public:
	BlockFunctionalityDialog(
		QWidget* parent, 
		const QString& title, 
		const QString& message,
		Qt::WindowFlags flags = Qt::Dialog | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

	virtual void showEvent(QShowEvent* event) override;

private:
	Ui_BlockFunctionalityDialog m_ui;
};

