#include "stdafx.h"
#include "header_toolbutton_creator.h"
#include "helpers.h"

namespace SeoSpider
{

QWidget* HeaderToolButtonCreator::createControl(QAction* action)
{
	QToolButton* button = new QToolButton;

	auto updateControl = [action, button]()
	{
		button->setToolTip(action->text());
		button->setIcon(action->icon());
		button->setEnabled(action->isEnabled());
		button->setIconSize(QSize(Common::Helpers::pointsToPixels(20), Common::Helpers::pointsToPixels(20)));
	};

	VERIFY(QObject::connect(button, &QToolButton::clicked, action, &QAction::trigger));
	VERIFY(QObject::connect(action, &QAction::changed, updateControl));

	updateControl();
	return button;
}

}