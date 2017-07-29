#include "naviagation_panel_controller.h"

namespace QuickieWebBot
{

NavigationPanelController::NavigationPanelController(QObject* parent, Ui::MainFrameClass* ui)
	: QObject(parent)
	, m_ui(ui)
	, m_prevSelectedButton(nullptr)
{
	m_ui->ceoAnalysisButton->setProperty("selected", true);
	m_ui->mainGuiStackedWidget->setCurrentIndex(1);
	m_prevSelectedButton = m_ui->ceoAnalysisButton;

	m_ui->siteStructureButton->installEventFilter(this);
	m_ui->ceoAnalysisButton->installEventFilter(this);
	m_ui->allPagesButton->installEventFilter(this);
	m_ui->allResourcesButton->installEventFilter(this);
	m_ui->domainMetricsButton->installEventFilter(this);
	m_ui->reportsButton->installEventFilter(this);
}

bool NavigationPanelController::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		if (QPushButton* button = qobject_cast<QPushButton*>(watched))
		{
			if (m_prevSelectedButton == button)
			{
				return false;
			}

			if (button->property("childElement").toBool() == false)
			{
				if (button == m_ui->siteStructureButton)
				{
					m_ui->siteStructurePanel->setVisible(!m_ui->siteStructurePanel->isVisible());
					return false;
				}
			}

			if (m_prevSelectedButton)
			{
				m_prevSelectedButton->setProperty("selected", false);
				m_prevSelectedButton->repaint();
				m_prevSelectedButton->style()->unpolish(m_prevSelectedButton);
				m_prevSelectedButton->style()->polish(m_prevSelectedButton);
			}
			
			m_prevSelectedButton = button;
			m_prevSelectedButton->setProperty("selected", true);
			m_prevSelectedButton->style()->unpolish(m_prevSelectedButton);
			m_prevSelectedButton->style()->polish(m_prevSelectedButton);

			int index = 1;
			if (m_prevSelectedButton == m_ui->allPagesButton)
			{
				index = 0;
			}
			else if (m_prevSelectedButton == m_ui->ceoAnalysisButton)
			{
				index = 1;
			}
			else if (m_prevSelectedButton == m_ui->allResourcesButton)
			{
				index = 2;
			}
			else if (m_prevSelectedButton == m_ui->domainMetricsButton)
			{
				index = 3;
			}
			else if (m_prevSelectedButton == m_ui->reportsButton)
			{
				index = 4;
			}

			m_ui->mainGuiStackedWidget->setCurrentIndex(index);

		}
	}

	return false;
}

}