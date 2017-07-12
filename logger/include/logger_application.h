#pragma once

class Application : public QApplication
{
	Q_OBJECT

public:
	static Application* instance();
	Application(int& argc, char** argv);


private:
	static Application* s_app;

};