#pragma once

#include "commands.h"

namespace SeoSpider
{

class IMenuItem
{
public:
	enum Type
	{
		SeparatorType,
		CommandType,
		MenuType
	};

	virtual ~IMenuItem() noexcept = default;

	virtual void addItem(Common::counted_ptr<IMenuItem> item)
	{
		Q_UNUSED(item);

		ASSERT(!"Not implemented");
	}

	virtual Type type() const noexcept = 0;
};

class SeparatorMenuItem : public IMenuItem
{
public:
	virtual Type type() const noexcept override
	{
		return IMenuItem::SeparatorType;
	}
};

class CommandMenuItem : public IMenuItem
{
public:
	CommandMenuItem(const ICommandPointer& commandPointer)
		: m_commandPointer(commandPointer)
	{
	}

	CommandMenuItem(ICommandPointer&& commandPointer)
		: m_commandPointer(std::move(commandPointer))
	{
	}

	virtual Type type() const noexcept override
	{
		return IMenuItem::CommandType;
	}

	ICommandPointer command() const noexcept
	{
		return m_commandPointer;
	}

private:
	ICommandPointer m_commandPointer;
};

class Menu : public IMenuItem
{
private:
	using Items = std::vector<Common::counted_ptr<IMenuItem>>;

public:
	using iterator = Items::iterator;
	using const_iterator = Items::const_iterator;

	Menu(const QString& name = QString())
		: m_name(name)
	{
	}

	virtual Type type() const noexcept override
	{
		return IMenuItem::MenuType;
	}

	const QString& name() const noexcept
	{
		return m_name;
	}

	virtual void addItem(Common::counted_ptr<IMenuItem> item) override
	{
		DEBUG_ASSERT(std::find(m_items.begin(), m_items.end(), item) == m_items.end() && "This item already exists");

		m_items.emplace_back(std::move(item));
	}

	std::size_t size() const noexcept
	{
		return m_items.size();
	}

	bool empty() const noexcept
	{
		return m_items.empty();
	}

	Common::counted_ptr<IMenuItem>& operator[](std::size_t idx) noexcept
	{
		return m_items[idx];
	}

	const Common::counted_ptr<IMenuItem>& operator[](std::size_t idx) const noexcept
	{
		return m_items[idx];
	}

	iterator begin() noexcept
	{
		return m_items.begin();
	}

	iterator end() noexcept
	{
		return m_items.end();
	}

	const_iterator begin() const noexcept
	{
		return m_items.begin();
	}

	const_iterator end() const noexcept
	{
		return m_items.end();
	}

	const_iterator cbegin() const noexcept
	{
		return m_items.begin();
	}

	const_iterator cend() const noexcept
	{
		return m_items.end();
	}

private:
	Items m_items;
	QString m_name;
};

class IMenuDataProvider
{
public:
	virtual ~IMenuDataProvider() = default;

	virtual Menu menuFor(const QModelIndex& index) const = 0;
};

}