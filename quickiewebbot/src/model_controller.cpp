#include "model_controller.h"

namespace QuickieWebBot
{

ModelController::ModelController(QObject* parent)
	: QObject(parent)
{
}

void ModelController::setUrl(QUrl const& url)
{
	m_url = url;
}

QUrl const& ModelController::url() const
{
	return m_url;
}

}