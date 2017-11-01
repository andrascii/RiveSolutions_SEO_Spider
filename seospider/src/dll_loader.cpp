#include "dll_loader.h"

namespace SeoSpider
{

DllLoader::~DllLoader()
{
	for (auto moduleInfoIterator = m_modules.begin(); moduleInfoIterator != m_modules.end(); ++moduleInfoIterator)
	{
		FreeLibrary(moduleInfoIterator.value());
	}

	m_modules.clear();
}

void DllLoader::load(const QByteArray& dllName)
{
	if (isLoaded(dllName))
	{
		return;
	}

	const HMODULE module = LoadLibraryA(dllName);

	if (!module)
	{
		qDebug() << "Can't load " << dllName;
	}
	else
	{
		m_modules[dllName] = module;
	}
}

void DllLoader::free(const QByteArray& dllName)
{
	if (!isLoaded(dllName))
	{
		return;
	}

	FreeLibrary(m_modules[dllName]);
	m_modules.remove(dllName);
}

bool DllLoader::isLoaded(const QByteArray& dllName) const
{
	return m_modules.contains(dllName);
}

}