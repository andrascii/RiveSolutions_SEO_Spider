#include "crawler_pause_settings_widget.h"

namespace SeoSpider
{

	CrawlerPauseSettingsWidget::CrawlerPauseSettingsWidget(QWidget* parent)
		: SettingsPage(parent)
	{
		m_ui.setupUi(this);

		init();

		VERIFY(connect(m_ui.fromPauseTimerSpinBox, SIGNAL(valueChanged(int)),
			this, SLOT(fromCrawlerPauseSpinBoxChanged(int))));

		VERIFY(connect(m_ui.toPauseTimerSpinBox, SIGNAL(valueChanged(int)),
			this, SLOT(toCrawlerPauseSpinBoxChanged(int))));
	}

	void CrawlerPauseSettingsWidget::init()
	{
		m_ui.label->installEventFilter(this);

		SettingsPage::init();

		fromCrawlerPauseSpinBoxChanged(m_ui.fromPauseTimerSpinBox->value());
		toCrawlerPauseSpinBoxChanged(m_ui.toPauseTimerSpinBox->value());

		m_ui.fromPauseTimerSpinBox->setEnabled(m_ui.usePauseTimerCheckBox->isChecked());
		m_ui.toPauseTimerSpinBox->setEnabled(m_ui.usePauseTimerCheckBox->isChecked());

	}

	void CrawlerPauseSettingsWidget::fromCrawlerPauseSpinBoxChanged(int value)
	{
		m_ui.toPauseTimerSpinBox->setMinimum(value);
	}

	void CrawlerPauseSettingsWidget::toCrawlerPauseSpinBoxChanged(int value)
	{
		m_ui.fromPauseTimerSpinBox->setMaximum(value);
	}

	bool CrawlerPauseSettingsWidget::eventFilter(QObject* object, QEvent* event)
	{
		if (object == m_ui.label && event->type() == QEvent::MouseButtonRelease)
		{
			m_ui.usePauseTimerCheckBox->toggle();
		}

		return false;
	}

}