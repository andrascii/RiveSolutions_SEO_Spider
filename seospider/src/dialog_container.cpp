#include "dialog_container.h"
#include "dialog.h"
#include "transparent_dialog_container.h"

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

int DialogContainer::openDialog(Dialog* dialog) const
{
	QDialog* container = new TransparentDialogContainer(dialog);
	//container->setAttribute(Qt::WA_DeleteOnClose, true);

	return container->exec();
}

}