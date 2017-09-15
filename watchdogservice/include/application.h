#pragma once

namespace WatchDogService
{

class Application : public QApplication
{
public:
	Application(int& argc, char** argv);

private:
	QString commandLineParameter(int num) const noexcept;

private:
	int& m_argc;
	char** m_argv;
};

}
