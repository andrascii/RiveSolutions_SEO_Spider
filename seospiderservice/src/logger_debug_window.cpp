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
    QString string = "<div style='color:" + s_textColors[message.severityLevel] + 
        "; background-color:" + s_backgroundColors[message.severityLevel] + ";'>" + message.message + "</div>\n";

    m_messages[AllLevels].push_back(string);
    m_messages[static_cast<int>(message.severityLevel)].push_back(string);

    textBrowser->append(string);
}

void LoggerDebugWindow::levelChanged()
{
    int messageType = severityLevelComboBox->currentData().toInt();

    QString string;

    for (const QString& msg : m_messages[messageType])
    {
        string += msg;
    }
    
    textBrowser->setText(string);
}

}