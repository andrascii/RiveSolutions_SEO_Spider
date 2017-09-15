#include "application.h"

namespace WatchDogService
{

Application::Application(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_argc(argc)
	, m_argv(argv)
{
}

QString Application::commandLineParameter(int num) const noexcept
{
	if (argc < 2 || num < 0)
	{
		return QString();
	}


}

}