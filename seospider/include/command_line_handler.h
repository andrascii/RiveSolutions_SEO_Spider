#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class CommandLineHandler
{
public:
	CommandLineHandler(int& argc, char** argv);
	QString commandValue(const char* commandKey) const;
	bool hasCommand(const char* commandKey) const;

private:
	boost::program_options::options_description m_optionsDescription;
	boost::program_options::variables_map m_variablesMap;
};

}