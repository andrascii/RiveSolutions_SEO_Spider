#include "dialog_container.h"
#include "dialog.h"
#include "floating_dialog.h"
#include "transparent_dialog_container.h"
#include "helpers.h"

namespace SeoSpider
{

DialogContainer& DialogContainer::instance()
{
	static std::unique_ptr<DialogContainer> s_instance;

	if (!s_instance)
	{
		DialogContainer* dialogContainer = new DialogContainer;

		try
		{
			s_instance.reset(dialogContainer);
		}
		catch (const std::bad_alloc&)
		{
			throw;
		}
		catch (...)
		{
			delete dialogContainer;
		}
	}

	return *s_instance;
}

void DialogContainer::openDialog(QWidget* widget) const
{
	const auto iter = m_dialogContainers.find(widget);

	ASSERT(iter != m_dialogContainers.end());

	iter->second->exec();
}

void DialogContainer::registerDialog(Dialog* dialog)
{
	ASSERT(m_dialogContainers.find(dialog) == m_dialogContainers.end());
	m_dialogContainers[dialog] = new TransparentDialogContainer(dialog);

	VERIFY(connect(dialog, SIGNAL(destroyed(QObject*)), this, SLOT(onDialogDestroyed(QObject*))));
}

void DialogContainer::registerDialog(FloatingDialog* dialog)
{
	ASSERT(m_dialogContainers.find(dialog) == m_dialogContainers.end());
	m_dialogContainers[dialog] = new TransparentDialogContainer(dialog);

	VERIFY(connect(dialog, SIGNAL(destroyed(QObject*)), this, SLOT(onDialogDestroyed(QObject*))));
}

void DialogContainer::onDialogDestroyed(QObject* object)
{
	QWidget* widget = Common::Helpers::fast_cast<QWidget*>(object);

	ASSERT(widget);

	m_dialogContainers.erase(widget);
}

}