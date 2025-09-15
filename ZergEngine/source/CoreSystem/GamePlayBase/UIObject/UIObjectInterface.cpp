#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

using namespace ze;

IUIObject::IUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: m_transform(this)
	, m_id(id)
	, m_tableIndex((std::numeric_limits<uint32_t>::max)())
	, m_groupIndex((std::numeric_limits<uint32_t>::max)())
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
	// 지연된 오브젝트를 제거하는 경우는 OnLoadScene에서 Destroy를 한다는 의미인데 이것은 허용하지 않는다.
	if (this->IsPending())
		return;

	UIObjectManager::GetInstance()->RequestDestroy(this);
}

void IUIObject::SetActive(bool active)
{
	for (RectTransform* pChildTransform : m_transform.m_children)
		pChildTransform->m_pUIObject->SetActive(active);

	UIObjectManager::GetInstance()->SetActive(this, active);
}

void IUIObject::OnDetachedFromUIInteraction()
{
	UIObjectManager* pUIObjectManager = UIObjectManager::GetInstance();

	if (pUIObjectManager->GetLButtonDownObject() == this)
		pUIObjectManager->SetLButtonDownObject(nullptr);

	if (pUIObjectManager->GetMButtonDownObject() == this)
		pUIObjectManager->SetMButtonDownObject(nullptr);

	if (pUIObjectManager->GetRButtonDownObject() == this)
		pUIObjectManager->SetRButtonDownObject(nullptr);
}

void IUIObject::OnLButtonDown()
{
	UIObjectManager* pUIObjectManager = UIObjectManager::GetInstance();

	pUIObjectManager->SetLButtonDownObject(this);
	pUIObjectManager->SetActiveInputField(nullptr);
}

void IUIObject::OnLButtonUp()
{
	UIObjectManager* pUIObjectManager = UIObjectManager::GetInstance();

	IUIObject* pLButtonDownObject = pUIObjectManager->GetLButtonDownObject();

	if (pLButtonDownObject == this)
	{
		this->OnLButtonClick();
	}
	else
	{
		if (pLButtonDownObject)
			pLButtonDownObject->OnLButtonUp();
	}

	pUIObjectManager->SetLButtonDownObject(nullptr);
}

void IUIObject::OnMButtonDown()
{
	UIObjectManager* pUIObjectManager = UIObjectManager::GetInstance();

	pUIObjectManager->SetMButtonDownObject(this);
	pUIObjectManager->SetActiveInputField(nullptr);
}

void IUIObject::OnMButtonUp()
{
	UIObjectManager* pUIObjectManager = UIObjectManager::GetInstance();

	IUIObject* pMButtonDownObject = pUIObjectManager->GetMButtonDownObject();

	if (pMButtonDownObject == this)
	{
		this->OnMButtonClick();
	}
	else
	{
		if (pMButtonDownObject)
			pMButtonDownObject->OnMButtonUp();
	}

	pUIObjectManager->SetMButtonDownObject(nullptr);
}

void IUIObject::OnRButtonDown()
{
	UIObjectManager* pUIObjectManager = UIObjectManager::GetInstance();

	pUIObjectManager->SetRButtonDownObject(this);
	pUIObjectManager->SetActiveInputField(nullptr);
}

void IUIObject::OnRButtonUp()
{
	UIObjectManager* pUIObjectManager = UIObjectManager::GetInstance();

	IUIObject* pRButtonDownObject = pUIObjectManager->GetRButtonDownObject();

	if (pRButtonDownObject == this)
	{
		this->OnRButtonClick();
	}
	else
	{
		if (pRButtonDownObject)
			pRButtonDownObject->OnRButtonUp();
	}

	pUIObjectManager->SetRButtonDownObject(nullptr);
}

const UIObjectHandle IUIObject::ToHandle() const
{
	assert(UIObjectManager::GetInstance()->m_handleTable[m_tableIndex] == this);

	return UIObjectHandle(m_tableIndex, m_id);
}