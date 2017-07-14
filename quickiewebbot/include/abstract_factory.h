#pragma once

namespace QuickieWebBot
{

template<typename T>
class AbstractCreator
{
public:
	virtual T* create() = 0;
};

template<typename Base, typename CreateType>
class ConcreteCreator : public AbstractCreator<Base>
{
public:
	virtual Base* create() override
	{
		return new CreateType;
	}
};

template<typename T>
class AbstractFactory final
{
public:
	T* create(std::string const& id)
	{
		auto iter = m_ids.find(id);

		if (iter != m_ids.end())
		{
			return iter->second->create();
		}

		return nullptr;
	}

	template<typename U>
	bool registrate(std::string const& id)
	{
		static_assert(std::is_base_of<T, U>::value,
			"The specified class type must be derived from type for which factory was created!");

		auto iter = m_ids.find(id);

		if (iter != m_ids.end())
		{
			return false;
		}

		m_ids[id] = new ConcreteCreator<T, U>;

		return true;
	}

private:
	std::map<std::string, AbstractCreator<T>*> m_ids;
};

}
