#pragma once

namespace Common
{

class ServiceLocator
{
public:
	static inline ServiceLocator* instance()
	{
		static std::unique_ptr<ServiceLocator, ServiceLocatorDeleter> s_instance = nullptr;

		if (!s_instance.get())
		{
			s_instance.reset(new ServiceLocator);
		}

		return s_instance.get();
	}

	template<typename ServiceType, typename ServiceImpl>
	void addService(ServiceImpl* service)
	{
		static_assert(std::is_base_of<ServiceType, ServiceImpl>::value, "Type of pointer must be the same type or derived from");

		assert(m_services.find(typeid(ServiceType).name()) == m_services.end());

		m_services[typeid(ServiceType).name()] = std::make_pair(service, &internalDeleter<ServiceType>);
	}

	template<typename ServiceType>
	void destroyService()
	{
		assert(m_services.find(typeid(ServiceType).name()) != m_services.end());

		auto findIterator = m_services.find(typeid(ServiceType).name());

		std::pair<void*, void(*)(void*)> serviceInfo = findIterator->second;

		void(*deleter)(void*) = std::get<1>(serviceInfo);
		void* service = std::get<0>(serviceInfo);

		deleter(service);

		m_services.erase(findIterator);
	}

	template<typename ServiceType>
	ServiceType* service() const
	{
		auto findIterator = m_services.find(typeid(ServiceType).name());

		assert(findIterator != m_services.end());

		return static_cast<ServiceType*>(std::get<0>(std::get<1>(*findIterator)));
	}

private:
	ServiceLocator() = default;
	ServiceLocator(ServiceLocator const&&) = delete;
	ServiceLocator(ServiceLocator&&) = delete;

	~ServiceLocator()
	{
		for (auto item : m_services)
		{
			void(*deleter)(void*) = std::get<1>(std::get<1>(item));
			void* service = std::get<0>(std::get<1>(item));

			deleter(service);
		}

		m_services.clear();
	}

	template<typename T>
	static void internalDeleter(void* pointer)
	{
		delete static_cast<T*>(pointer);
	}

	struct ServiceLocatorDeleter
	{
		void operator()(ServiceLocator* serviceLocator)
		{
			delete serviceLocator;
		}
	};

private:
	std::unordered_map<std::string, std::pair<void*, void(*)(void*)>> m_services;
};

}