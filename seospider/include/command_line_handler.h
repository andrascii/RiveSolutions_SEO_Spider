#pragma once

namespace SeoSpider
{

class CommandLineHandler
{
public:
	CommandLineHandler(int& argc, char** argv);
	QString getCommandArguments(const char*);

private:
	boost::program_options::options_description m_optionsDescription;
	boost::program_options::variables_map m_variablesMap;
};

}