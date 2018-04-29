#pragma once

#include "ui_logger_debug_window.h"

namespace Common
{

struct PipeMessage;

}

namespace SeoSpiderService
{

class LoggerDebugWindow : public QMainWindow
{
	Q_OBJECT

private:
	using ColoredString = std::tuple<QColor /* background color */, QColor /* text color */, QString>;

public:
	LoggerDebugWindow(QWidget* parent = nullptr);

	Q_SLOT void onMessageReceived(const Common::PipeMessage& message);

protected:
	virtual void closeEvent(QCloseEvent* event) override;

private:
	Q_SLOT void levelChanged();

	void loadState();

private:
	enum
	{
		AllLevels = -1
	};

	std::map<int, std::vector<ColoredString>> m_messages;

	Ui_LoggerDebugWindow* m_ui;
};

}