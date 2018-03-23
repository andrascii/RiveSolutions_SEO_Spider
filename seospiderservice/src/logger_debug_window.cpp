#include "logger_debug_window.h"
#include "pipe_message.h"

namespace
{

std::map<Common::SeverityLevel, QString> s_backgroundColors =
{
	{ Common::SeverityLevel::TraceLevel, "#293955" },
	{ Common::SeverityLevel::DebugLevel, "#007ACC" },
	{ Common::SeverityLevel::InfoLevel, "#FFFFFF" },
	{ Common::SeverityLevel::WarningLevel, "#F2CF55" },
	{ Common::SeverityLevel::ErrorLevel, "#A40024" },
};

std::map<Common::SeverityLevel, QString> s_textColors =
{
	{ Common::SeverityLevel::TraceLevel, "#EDEDED" },
	{ Common::SeverityLevel::DebugLevel, "#EDEDED" },
	{ Common::SeverityLevel::InfoLevel, "#000000" },
	{ Common::SeverityLevel::WarningLevel, "#000000" },
	{ Common::SeverityLevel::ErrorLevel, "#FFFFFF" },
};

}

namespace SeoSpiderService
{

LoggerDebugWindow::LoggerDebugWindow(QWidget* parent)
	: QWidget(parent)
{
	setupUi(this);

	severityLevelComboBox->addItem(QStringLiteral("All levels"), QVariant::fromValue(static_cast<int>(AllLevels)));
	severityLevelComboBox->addItem(QStringLiteral("Trace level"), QVariant::fromValue(Common::SeverityLevel::TraceLevel));
	severityLevelComboBox->addItem(QStringLiteral("Debug level"), QVariant::fromValue(Common::SeverityLevel::DebugLevel));
	severityLevelComboBox->addItem(QStringLiteral("Info level"), QVariant::fromValue(Common::SeverityLevel::InfoLevel));
	severityLevelComboBox->addItem(QStringLiteral("Warning level"), QVariant::fromValue(Common::SeverityLevel::WarningLevel));
	severityLevelComboBox->addItem(QStringLiteral("Error level"), QVariant::fromValue(Common::SeverityLevel::ErrorLevel));
	severityLevelComboBox->setCurrentIndex(0);

	connect(severityLevelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(levelChanged()));
}

void LoggerDebugWindow::onMessageReceived(const Common::PipeMessage& message)
{
	Common::SeverityLevel level = static_cast<Common::SeverityLevel>(message.severityLevel);

	QString messageString;

	QTextStream stream(&messageString);

	QString messageType;

	if (message.type == Common::PipeMessage::Assert)
	{
		messageType = "Assertion failure: ";
	}
	else if (message.type == Common::PipeMessage::Assert)
	{
		messageType = "Crash: ";
	}

	stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
		<< QChar(';') << message.threadId
		<< QChar(';') << message.severityLevel
		<< QChar(';') << message.line
		<< QChar(';') << message.file
		<< QChar(';') << message.function
		<< QChar(';') << messageType << message.message;

	stream.flush();

	auto internalMessage = std::make_tuple(s_backgroundColors[level], s_textColors[level], messageString);

	m_messages[AllLevels].push_back(internalMessage);
	m_messages[message.severityLevel].push_back(internalMessage);

	textBrowser->setTextBackgroundColor(QColor(s_backgroundColors[level]));
	textBrowser->setTextColor(QColor(s_textColors[level]));
	textBrowser->append(messageString);
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