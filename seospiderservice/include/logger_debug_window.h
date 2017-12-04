#pragma once

#include "ui_logger_debug_window.h"
#include "log_message_receiver.h"

namespace SeoSpiderService
{

class LoggerDebugWindow : public QWidget, protected Ui_LoggerDebugWindow
{
	Q_OBJECT

private:
	using ColoredString = std::tuple<QColor /* background color */, QColor /* text color */, QString>;

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

	std::map<int, std::vector<ColoredString>> m_messages;
};

}