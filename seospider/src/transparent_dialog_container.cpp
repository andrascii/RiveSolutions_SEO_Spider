#include "transparent_dialog_container.h"
#include "shadow_decoration_frame.h"
#include "helpers.h"
#include "dialog.h"
#include "floating_dialog.h"
#include "cursor_factory.h"

using namespace SeoSpider;

TransparentDialogContainer::TransparentDialogContainer(Dialog* dialog)
	: TransparentDialogContainer(static_cast<QWidget*>(dialog))
{
	m_isFloating = false;

	VERIFY(connect(dialog, &Dialog::dialogClosed, this, &TransparentDialogContainer::done));
}

TransparentDialogContainer::TransparentDialogContainer(FloatingDialog* dialog)
	: TransparentDialogContainer(static_cast<QWidget*>(dialog))
{
	m_isFloating = true;

	VERIFY(connect(dialog, &FloatingDialog::dialogClosed, this, &TransparentDialogContainer::done));

	setCursor(CursorFactory::createCursor(Qt::OpenHandCursor));
}

TransparentDialogContainer::TransparentDialogContainer(QWidget* dialog)
	: QDialog(dialog->parentWidget())
	, m_widget(dialog)
	, m_mousePressed(false)
{
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(new ShadowDecorationFrame(dialog, this));

	resize(size());
}

void TransparentDialogContainer::showEvent(QShowEvent* event)
{
	adjustSize();

	QDialog::showEvent(event);
}

void TransparentDialogContainer::mousePressEvent(QMouseEvent* event)
{
	if (!m_isFloating)
	{
		return;
	}

	setCursor(CursorFactory::createCursor(Qt::ClosedHandCursor));

	m_mousePressed = true;
	m_mousePos = event->globalPos();
	m_wndPos = pos();
}

void TransparentDialogContainer::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isFloating && m_mousePressed)
	{
		move(m_wndPos + (event->globalPos() - m_mousePos));
	}
}

void TransparentDialogContainer::mouseReleaseEvent(QMouseEvent*)
{
	if (!m_isFloating)
	{
		return;
	}

	m_mousePressed = false;

	setCursor(CursorFactory::createCursor(Qt::OpenHandCursor));
}
