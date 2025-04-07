#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GameObjectManager.h>

using namespace ze;

GameObject::GameObject(bool deferred, PCWSTR name)
	: m_transform()
	, m_components()
	, m_id(GameObjectManager.AssignUniqueId())
	, m_index(std::numeric_limits<uint32_t>::max())
	, m_activeIndex(std::numeric_limits<uint32_t>::max())
	, m_flag(deferred ? GOF_DEFERRED : GOF_NONE)
{
	StringCbCopyW(m_name, sizeof(m_name), name);
}

GameObjectHandle GameObject::ToHandle()
{
	if (GameObjectManager.m_ptrTable[m_index] == this)
		return GameObjectHandle(m_index, m_id);
	else
		return GameObjectHandle();	// Invalid handle
}
