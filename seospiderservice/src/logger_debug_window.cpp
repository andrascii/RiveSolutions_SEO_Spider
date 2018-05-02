#include "logger_debug_window.h"
#include "pipe_message.h"
#include "version.h"

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
	: QMainWindow(parent)
	, m_ui(new Ui_LoggerDebugWindow)
{
	QWidget* centralWidget = new QWidget(this);
	m_ui->setupUi(centralWidget);

	setCentralWidget(centralWidget);

	m_ui->severityLevelComboBox->addItem(QStringLiteral("All levels"), QVariant::fromValue(static_cast<int>(AllLevels)));
	m_ui->severityLevelComboBox->addItem(QStringLiteral("Trace level"), QVariant::fromValue(Common::SeverityLevel::TraceLevel));
	m_ui->severityLevelComboBox->addItem(QStringLiteral("Debug level"), QVariant::fromValue(Common::SeverityLevel::DebugLevel));
	m_ui->severityLevelComboBox->addItem(QStringLiteral("Info level"), QVariant::fromValue(Common::SeverityLevel::InfoLevel));
	m_ui->severityLevelComboBox->addItem(QStringLiteral("Warning level"), QVariant::fromValue(Common::SeverityLevel::WarningLevel));
	m_ui->severityLevelComboBox->addItem(QStringLiteral("Error level"), QVariant::fromValue(Common::SeverityLevel::ErrorLevel));
	m_ui->severityLevelComboBox->setCurrentIndex(0);

	connect(m_ui->severityLevelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(levelChanged()));

	loadState();
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

	m_ui->textBrowser->setTextBackgroundColor(QColor(s_backgroundColors[level]));
	m_ui->textBrowser->setTextColor(QColor(s_textColors[level]));
	m_ui->textBrowser->append(messageString);
}

void LoggerDebugWindow::closeEvent(QCloseEvent* event)
{
	QSettings settings("Rive Solutions", "Logger");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState(MAINTENANCE));
	QWidget::closeEvent(event);
}

void LoggerDebugWindow::levelChanged()
{
	m_ui->textBrowser->clear();

	int messageType = m_ui->severityLevelComboBox->currentData().toInt();

	for (const ColoredString& msg : m_messages[messageType])
	{
		m_ui->textBrowser->setTextBackgroundColor(std::get<0>(msg));
		m_ui->textBrowser->setTextColor(std::get<1>(msg));
		m_ui->textBrowser->append(std::get<2>(msg));
	}
}

void LoggerDebugWindow::loadState()
{
	QSettings settings("Rive Solutions", "Logger");

	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
}

}