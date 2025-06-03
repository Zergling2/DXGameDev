#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

using namespace ze;

IUIObject::IUIObject(UIOBJECT_FLAG flag, PCWSTR name, UIOBJECT_TYPE type)
	: m_transform(this)
	, m_id(UIObjectManager.AssignUniqueId())
	, m_tableIndex(std::numeric_limits<uint32_t>::max())
	, m_activeIndex(std::numeric_limits<uint32_t>::max())
	, m_flag(flag)
	, m_type(type)
{
	StringCbCopyW(m_name, sizeof(m_name), name);
}

void IUIObject::SetActive(bool active)
{
	for (RectTransform* pChildTransform : m_transform.m_children)
		pChildTransform->m_pUIObject->SetActive(active);

	// 이미 해당 활성 상태가 설정되어 있는 경우 함수 리턴
	const bool isActive = this->IsActive();
	if (isActive == active)
		return;

	if (active)
	{
		this->OnFlag(UIOBJECT_FLAG::ACTIVE);

		// 지연되지 않은 경우에만 포인터 이동 (지연된 상태에서는 Active/Inactive 벡터에 포인터가 존재하지 않는다.)
		if (!this->IsDeferred())
			UIObjectManager.MoveToActiveVectorFromInactiveVector(this);
	}
	else
	{
		this->OffFlag(UIOBJECT_FLAG::ACTIVE);

		// 지연되지 않은 경우에만 포인터 이동 (지연된 상태에서는 Active/Inactive 벡터에 포인터가 존재하지 않는다.)
		if (!this->IsDeferred())
			UIObjectManager.MoveToInactiveVectorFromActiveVector(this);
	}
}

const UIObjectHandle IUIObject::ToHandleBase() const
{
	assert(UIObjectManager.m_table[m_tableIndex] == this);

	return UIObjectHandle(m_tableIndex, m_id);
}

UIObjectHandle IUIObject::CreateChildUIObjectImpl(IUIObject* pUIObject)
{
	UIObjectHandle hUIObject = UIObjectManager.RegisterToHandleTable(pUIObject);

	if (!pUIObject->IsDeferred())
		UIObjectManager.AddPtrToActiveVector(pUIObject);

	return hUIObject;
}
