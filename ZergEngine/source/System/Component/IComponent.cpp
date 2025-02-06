#include <ZergEngine\System\Component\IComponent.h>

using namespace zergengine;

IComponent::~IComponent()
{
}

bool IComponent::Enable()
{
	bool currentVal = m_enabled;

	if (m_enabled == false)
	{
		m_enabled = true;
		SystemJobOnEnabled();
	}

	return currentVal;
}

bool IComponent::Disable()
{
	bool currentVal = m_enabled;

	if (m_enabled == true)
	{
		m_enabled = false;
		SystemJobOnDisabled();
	}

	return currentVal;
}