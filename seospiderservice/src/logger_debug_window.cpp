#include "logger_debug_window.h"

namespace
{

std::map<SeoSpiderServiceApi::SeverityLevel, QString> s_backgroundColors =
{
	{ SeoSpiderServiceApi::SeverityLevel::TraceLevel, "#293955" },
	{ SeoSpiderServiceApi::SeverityLevel::DebugLevel, "#007ACC" },
	{ SeoSpiderServiceApi::SeverityLevel::InfoLevel, "#FFFFFF" },
	{ SeoSpiderServiceApi::SeverityLevel::WarningLevel, "#F2CF55" },
	{ SeoSpiderServiceApi::SeverityLevel::ErrorLevel, "#A40024" },
};

std::map<SeoSpiderServiceApi::SeverityLevel, QString> s_textColors =
{
	{ SeoSpiderServiceApi::SeverityLevel::TraceLevel, "#EDEDED" },
	{ SeoSpiderServiceApi::SeverityLevel::DebugLevel, "#EDEDED" },
	{ SeoSpiderServiceApi::SeverityLevel::InfoLevel, "#000000" },
	{ SeoSpiderServiceApi::SeverityLevel::WarningLevel, "#000000" },
	{ SeoSpiderServiceApi::SeverityLevel::ErrorLevel, "#FFFFFF" },
};

}

namespace SeoSpiderService
{

LoggerDebugWindow::LoggerDebugWindow(QWidget* parent)
	: QWidget(parent)
{
	setupUi(this);

	severityLevelComboBox->addItem(QStringLiteral("All levels"), QVariant::fromValue(static_cast<int>(AllLevels)));
	severityLevelComboBox->addItem(QStringLiteral("Trace level"), QVariant::fromValue(SeoSpiderServiceApi::SeverityLevel::TraceLevel));
	severityLevelComboBox->addItem(QStringLiteral("Debug level"), QVariant::fromValue(SeoSpiderServiceApi::SeverityLevel::DebugLevel));
	severityLevelComboBox->addItem(QStringLiteral("Info level"), QVariant::fromValue(SeoSpiderServiceApi::SeverityLevel::InfoLevel));
	severityLevelComboBox->addItem(QStringLiteral("Warning level"), QVariant::fromValue(SeoSpiderServiceApi::SeverityLevel::WarningLevel));
	severityLevelComboBox->addItem(QStringLiteral("Error level"), QVariant::fromValue(SeoSpiderServiceApi::SeverityLevel::ErrorLevel));
	severityLevelComboBox->setCurrentIndex(0);

	connect(severityLevelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(levelChanged()));
}

void LoggerDebugWindow::onMessageReceived(Message message)
{
	auto internalMessage = std::make_tuple(s_backgroundColors[message.severityLevel], s_textColors[message.severityLevel], message.message);

	m_messages[AllLevels].push_back(internalMessage);
	m_messages[static_cast<int>(message.severityLevel)].push_back(internalMessage);

	textBrowser->setTextBackgroundColor(QColor(s_backgroundColors[message.severityLevel]));
	textBrowser->setTextColor(QColor(s_textColors[message.severityLevel]));
	textBrowser->append(message.message);
}

void LoggerDebugWindow::levelChanged()
{
	textBrowser->clear();

	int messageType = severityLevelComboBox->currentData().toInt();

	for (const ColoredString& msg : m_messages[messageType])
	{
		textBrowser->setTextBackgroundColor(std::get<0>(msg));
		textBrowser->setTextColor(std::get<1>(msg));
		textBrowser->append(std::get<2>(msg));
	}
}

}