#include "block_functionality_dialog.h"

BlockFunctionalityDialog::BlockFunctionalityDialog(
	QWidget* parent,
	const QString& title,
	const QString& message,
	Qt::WindowFlags flags)
	: QDialog(parent, flags)
{
	QWidget* content = new QWidget(this);
	m_ui.setupUi(content);

	setWindowTitle(title);

	m_ui.messageLabel->setText(message);

	adjustSize();
}