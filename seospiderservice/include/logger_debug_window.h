#pragma once

#include "ui_logger_debug_window.h"
#include "log_message_receiver.h"

namespace SeoSpiderService
{

class LoggerDebugWindow : public QWidget, protected Ui_LoggerDebugWindow
{
    Q_OBJECT

public:
    LoggerDebugWindow(QWidget* parent = nullptr);

    Q_SLOT void onMessageReceived(Message message);

private:
    Q_SLOT void levelChanged();

private:
    enum
    {
        AllLevels = -1
    };

    std::map<int, std::vector<QString>> m_messages;
};

}