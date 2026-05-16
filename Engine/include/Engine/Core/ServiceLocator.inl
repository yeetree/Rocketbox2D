#include "Engine/Core/ServiceLocator.h"

#include <type_traits>

template<typename T>
void Engine::ServiceLocator::RegisterInstance(T* instance)
{
	const size_t hash = typeid(T).hash_code();
	if (m_Instances.find(hash) == m_Instances.end())
		m_Instances.emplace(hash, Ref<void>(instance));
}

template<typename T>
void Engine::ServiceLocator::RegisterCreator(std::function<Ref<T>()> creator)
{
	const size_t hash = typeid(T).hash_code();
	if (m_Creators.find(hash) == m_Creators.end())
		m_Creators.emplace(hash, creator);
}

template<typename T>
Engine::Ref<T> Engine::ServiceLocator::Get() const
{
	const size_t hash = typeid(T).hash_code();
	auto itr1 = m_Instances.find(hash);
	if (itr1 != m_Instances.end())
		return std::static_pointer_cast<T>(itr1->second);

	auto itr2 = m_Creators.find(hash);
	if (itr2 != m_Creators.end())
		return std::static_pointer_cast<T>(itr2->second());

	return nullptr;
}