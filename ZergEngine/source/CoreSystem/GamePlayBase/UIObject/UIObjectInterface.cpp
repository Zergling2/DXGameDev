#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

using namespace ze;

IUIObject::IUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: m_transform(this)
	, m_id(id)
	, m_tableIndex(std::numeric_limits<uint32_t>::max())
	, m_groupIndex(std::numeric_limits<uint32_t>::max())
	, m_flag(flag)
{
	StringCbCopyW(m_name, sizeof(m_name), name);
}

void IUIObject::DontDestroyOnLoad()
{
	this->OnFlag(UIOBJECT_FLAG::DONT_DESTROY_ON_LOAD);
}

void IUIObject::Destroy()
{
	UIObjectManager::GetInstance()->RequestDestroy(this);
}

void IUIObject::SetActive(bool active)
{
	for (RectTransform* pChildTransform : m_transform.m_children)
		pChildTransform->m_pUIObject->SetActive(active);

	UIObjectManager::GetInstance()->SetActive(this, active);
}

const UIObjectHandle IUIObject::ToHandleBase() const
{
	assert(UIObjectManager::GetInstance()->m_handleTable[m_tableIndex] == this);

	return UIObjectHandle(m_tableIndex, m_id);
}
