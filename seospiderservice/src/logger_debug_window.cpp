#include "logger_debug_window.h"
#include "version.h"

namespace
{

std::map<int, QString> s_backgroundColors =
{
	{ Common::LogLevel::DebugLog, "#007ACC" },
	{ Common::LogLevel::InfoLog, "#FFFFFF" },
	{ Common::LogLevel::WarningLog, "#F2CF55" },
	{ Common::LogLevel::ErrorLog, "#A40024" },
};

std::map<int, QString> s_textColors =
{
	{ Common::LogLevel::DebugLog, "#EDEDED" },
	{ Common::LogLevel::InfoLog, "#000000" },
	{ Common::LogLevel::WarningLog, "#000000" },
	{ Common::LogLevel::ErrorLog, "#FFFFFF" },
};

const QChar s_separator(';');

const int s_allLevels = 0;

}

namespace SeoSpiderService
{

LoggerDebugWindow::LoggerDebugWindow(QWidget* parent)
	: QMainWindow(parent)
	, m_currentLevel(s_allLevels)
	, m_ui(new Ui_LoggerDebugWindow)
{
	QWidget* centralWidget = new QWidget(this);
	m_ui->setupUi(centralWidget);

	setCentralWidget(centralWidget);

	m_ui->severityLevelComboBox->addItem(QStringLiteral("All levels"), QVariant::fromValue(s_allLevels));
	m_ui->severityLevelComboBox->addItem(QStringLiteral("Debug level"), QVariant::fromValue(Common::LogLevel::DebugLog));
	m_ui->severityLevelComboBox->addItem(QStringLiteral("Info level"), QVariant::fromValue(Common::LogLevel::InfoLog));
	m_ui->severityLevelComboBox->addItem(QStringLiteral("Warning level"), QVariant::fromValue(Common::LogLevel::WarningLog));
	m_ui->severityLevelComboBox->addItem(QStringLiteral("Error level"), QVariant::fromValue(Common::LogLevel::ErrorLog));
	m_ui->severityLevelComboBox->setCurrentIndex(0);

	connect(m_ui->severityLevelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(levelChanged()));

	loadState();
}

void LoggerDebugWindow::onCommandReceived(Common::Command command)
{
	int level = -1;
	QString messageString;
	QTextStream stream(&messageString);

	switch (command.type)
	{
		case Common::Command::Assert:
		{
			Common::AssertData* assertData = command.assertData();
			level = Common::LogLevel::ErrorLog;

			stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
				<< s_separator << assertData->file
				<< s_separator << assertData->line
				<< s_separator << assertData->function
				<< s_separator << quintptr(assertData->thisptr)
				<< s_separator << assertData->processId
				<< s_separator << quintptr(assertData->threadId)
				<< s_separator << "ASSERTION FAILURE: " << assertData->condition;
			break;
		}
		case Common::Command::Dump:
		{
			Common::DumpData* dumpData = command.dumpData();
			level = Common::LogLevel::ErrorLog;

			stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
				<< s_separator << dumpData->processId
				<< s_separator << quintptr(dumpData->threadId)
				<< s_separator << (dumpData->dumpParams == Common::DumpData::NativeCrash ? "Native crash" : "User dump");
			break;
		}
		case Common::Command::Log:
		{
			Common::LogData* logData = command.logData();
			level = logData->level;

			stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
				<< s_separator << logData->categoryId
				<< s_separator << logData->level
				<< s_separator << logData->file
				<< s_separator << logData->line
				<< s_separator << logData->function
				<< s_separator << quintptr(logData->thisptr)
				<< s_separator << quintptr(logData->threadId)
				<< s_separator << logData->text;
			break;
		}
		case Common::Command::LogCategory:
		{
			break;
		}
		case Common::Command::Restart:
		{
			Common::RestartData* restartData = command.restartData();
			level = Common::LogLevel::InfoLog;

			stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
				<< s_separator << "Restart request: " << restartData->message;
			break;
		}
		case Common::Command::Counter:
		case Common::Command::ApplicationInitialized:
		{
			return;
		}
	}

	stream.flush();

	auto internalMessage = std::make_tuple(s_backgroundColors[level], s_textColors[level], messageString);

	Q_ASSERT(level != -1);

	m_messages[s_allLevels].push_back(internalMessage);
	m_messages[level].push_back(internalMessage);

	if (m_currentLevel != s_allLevels &&
		m_currentLevel != level)
	{
		return;
	}

	m_ui->textBrowser->setTextBackgroundColor(QColor(s_backgroundColors[level]));
	m_ui->textBrowser->setTextColor(QColor(s_textColors[level]));
	m_ui->textBrowser->append(messageString);
}

void LoggerDebugWindow::closeEvent(QCloseEvent* event)
{
	QSettings settings("RiveSolutions", "Logger");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState(MAINTENANCE));
	QWidget::closeEvent(event);
}

void LoggerDebugWindow::levelChanged()
{
	m_ui->textBrowser->clear();

	const int messageType = m_ui->severityLevelComboBox->currentData().toInt();
	m_currentLevel = messageType;

	for (const ColoredString& msg : m_messages[messageType])
	{
		m_ui->textBrowser->setTextBackgroundColor(std::get<0>(msg));
		m_ui->textBrowser->setTextColor(std::get<1>(msg));
		m_ui->textBrowser->append(std::get<2>(msg));
	}
}

void LoggerDebugWindow::loadState()
{
	QSettings settings("RiveSolutions", "Logger");

	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
}

}