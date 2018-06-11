#include "transparent_dialog_container.h"
#include "shadow_decoration_frame.h"
#include "helpers.h"
#include "dialog.h"
#include "floating_dialog.h"

using namespace SeoSpider;

TransparentDialogContainer::TransparentDialogContainer(Dialog* dialog)
	: TransparentDialogContainer(static_cast<QWidget*>(dialog))
{
	VERIFY(connect(dialog, &Dialog::dialogClosed, this, &TransparentDialogContainer::done));
}

TransparentDialogContainer::TransparentDialogContainer(FloatingDialog* dialog)
	: TransparentDialogContainer(static_cast<QWidget*>(dialog))
{
	VERIFY(connect(dialog, &FloatingDialog::dialogClosed, this, &TransparentDialogContainer::done));
}

TransparentDialogContainer::TransparentDialogContainer(QWidget* dialog)
	: QDialog(dialog->parentWidget())
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
