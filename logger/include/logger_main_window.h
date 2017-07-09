#pragma once

#include "ui_logger_main_window.h"

class LoggerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	LoggerMainWindow(QWidget *parent = 0);


private:
	void init();

private slots:
	void sendMessage();

private:
	Ui::MainWindow ui;
	QLocalServer* m_server;
	QString message;

	//QLocalSocket* m_socket;
	//QString m_currentFortune;
	//quint32 m_blockSize;


};