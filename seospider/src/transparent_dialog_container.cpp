#include "transparent_dialog_container.h"
#include "shadow_decoration_frame.h"
#include "dialog.h"
#include "helpers.h"

using namespace SeoSpider;

TransparentDialogContainer::TransparentDialogContainer(Dialog* dialog)
	//: QFrame(dialog->parentWidget())
{
	//setAttribute(Qt::WA_TranslucentBackground);
	//setWindowFlags(Qt::FramelessWindowHint);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(new ShadowDecorationFrame(dialog, this));

	VERIFY(connect(dialog, &Dialog::dialogClosed, this, &TransparentDialogContainer::close));

	resize(size());
}

TransparentDialogContainer::~TransparentDialogContainer()
{
}

void TransparentDialogContainer::closeEvent(QCloseEvent*)
{
}
