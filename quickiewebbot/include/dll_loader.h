#pragma once

namespace QuickieWebBot
{

class DllLoader final
{
public:
	~DllLoader();

	void load(const QByteArray& dllName);
	void free(const QByteArray& dllName);
	bool isLoaded(const QByteArray& dllName) const;

private:
	QMap<QByteArray, HMODULE> m_modules;
};

}