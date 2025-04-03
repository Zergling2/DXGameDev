#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

using namespace ze;

IComponent::IComponent()
	: m_hGameObject()
	, m_id(std::numeric_limits<uint64_t>::max())
	, m_index(std::numeric_limits<uint32_t>::max())
	, m_activeIndex(std::numeric_limits<uint32_t>::max())
	, m_enabled(true)
{
}

bool IComponent::Enable()
{
	bool currentVal = m_enabled;

	if (m_enabled == false)
	{
		m_enabled = true;
		this->SystemJobOnEnabled();
	}

	return currentVal;
}

bool IComponent::Disable()
{
	bool currentVal = m_enabled;

	if (m_enabled == true)
	{
		m_enabled = false;
		this->SystemJobOnDisabled();
	}

	return currentVal;
}

void IComponent::SystemJobOnEnabled()
{
}

void IComponent::SystemJobOnDisabled()
{
}
