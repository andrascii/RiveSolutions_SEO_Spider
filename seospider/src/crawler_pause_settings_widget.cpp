#include "stdafx.h"
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

	VERIFY(connect(m_ui.usePauseTimerCheckBox, SIGNAL(stateChanged(int)),
		this, SLOT(useCrawlerPauseCheckBoxChanged(int))));

	VERIFY(connect(m_ui.usePauseTimerCheckBox, &QCheckBox::toggled, m_ui.label_2, &QWidget::setEnabled));
	VERIFY(connect(m_ui.usePauseTimerCheckBox, &QCheckBox::toggled, m_ui.label_3, &QWidget::setEnabled));

	// TODO: connect to theApp->preferences changes and update UI controls correspondingly
}

void CrawlerPauseSettingsWidget::init()
{
	SettingsPage::init();

	fromCrawlerPauseSpinBoxChanged(m_ui.fromPauseTimerSpinBox->value());
	toCrawlerPauseSpinBoxChanged(m_ui.toPauseTimerSpinBox->value());
	useCrawlerPauseCheckBoxChanged(m_ui.usePauseTimerCheckBox->checkState());

	m_ui.fromPauseTimerSpinBox->setEnabled(m_ui.usePauseTimerCheckBox->isChecked());
	m_ui.toPauseTimerSpinBox->setEnabled(m_ui.usePauseTimerCheckBox->isChecked());
	m_ui.label_2->setEnabled(m_ui.usePauseTimerCheckBox->isChecked());
	m_ui.label_3->setEnabled(m_ui.usePauseTimerCheckBox->isChecked());
}

void CrawlerPauseSettingsWidget::fromCrawlerPauseSpinBoxChanged(int value)
{
	m_ui.toPauseTimerSpinBox->setMinimum(value);
}

void CrawlerPauseSettingsWidget::toCrawlerPauseSpinBoxChanged(int value)
{
	m_ui.fromPauseTimerSpinBox->setMaximum(value);
}

void CrawlerPauseSettingsWidget::useCrawlerPauseCheckBoxChanged(int value)
{
	m_ui.toPauseTimerSpinBox->setEnabled(Qt::Checked == value);
	m_ui.fromPauseTimerSpinBox->setEnabled(Qt::Checked == value);
}

void CrawlerPauseSettingsWidget::onShow()
{
}

void CrawlerPauseSettingsWidget::onClose()
{
}

}