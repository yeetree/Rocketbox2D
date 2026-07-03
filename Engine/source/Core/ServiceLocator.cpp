#include "Engine/Core/ServiceLocator.h"

Engine::ServiceLocator::ServiceLocator() : m_Services()
{

}

Engine::ServiceLocator::~ServiceLocator()
{
    Clear();
}

void Engine::ServiceLocator::Clear()
{
    m_Services.clear();
}