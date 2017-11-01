#pragma once

namespace SeoSpider
{

class IDllLoader
{
public:
	virtual ~IDllLoader() = default;

	virtual void load(const QByteArray& dllName) = 0;
	virtual void free(const QByteArray& dllName) = 0;
	virtual bool isLoaded(const QByteArray& dllName) const = 0;
};

class DllLoader : public IDllLoader
{
public:
	virtual ~DllLoader();

	virtual void load(const QByteArray& dllName) override;
	virtual void free(const QByteArray& dllName) override;
	virtual bool isLoaded(const QByteArray& dllName) const override;

private:
	QMap<QByteArray, HMODULE> m_modules;
};

}