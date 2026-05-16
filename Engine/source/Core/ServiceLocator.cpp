#include "Engine/Core/ServiceLocator.h"

Engine::ServiceLocator::ServiceLocator() : m_Instances(), m_Creators() 
{

}

Engine::ServiceLocator::~ServiceLocator()
{
    Clear();
}

void Engine::ServiceLocator::Clear()
{
    m_Instances.clear();
	m_Creators.clear();
}