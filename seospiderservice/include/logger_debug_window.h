#pragma once

#include "ui_logger_debug_window.h"
#include "command.h"

namespace Common
{

struct Command;

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

	Q_SLOT void onCommandReceived(Common::Command command);

protected:
	virtual void closeEvent(QCloseEvent* event) override;

private:
	Q_SLOT void levelChanged();

	void loadState();

private:
	int m_currentLevel;
	std::map<int, std::vector<ColoredString>> m_messages;
	Ui_LoggerDebugWindow* m_ui;
};

}