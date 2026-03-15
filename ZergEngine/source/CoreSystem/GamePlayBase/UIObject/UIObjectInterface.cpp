#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

using namespace ze;

IUIObject::IUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: m_transform(this)
	, m_id(id)
	, m_tableIndex((std::numeric_limits<uint32_t>::max)())
	, m_actInactGroupIndex((std::numeric_limits<uint32_t>::max)())
	, m_flag(flag)
	, m_handlerOnClick()
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
	// 이미 해당 활성 상태가 설정되어 있는 경우 함수 리턴
	if (this->IsActiveSelf() == active)
		return;

	if (active)
		this->OnFlag(UIOBJECT_FLAG::ACTIVE_SELF);
	else
		this->OffFlag(UIOBJECT_FLAG::ACTIVE_SELF);

	RectTransform* pParentTransform = m_transform.m_pParent;
	if (pParentTransform)
		this->UpdateActiveState(pParentTransform->m_pUIObject->IsActiveInHierarchy());
	else
		this->UpdateActiveState(true);	// 부모가 없는 경우 부모의 ActiveInHierarchy가 true라고 가정하면 된다.
}

void IUIObject::OnLButtonClick(POINT pt)
{
	// 지연 객체(예시: OnLoadScene)인 경우 콜백 호출 방지
	if (this->IsPending())
		return;

	// 시각적 상태 업데이트 등... (ex. ImageButton)

	// UI Event Callback
	if (m_handlerOnClick)
	{
		bool success = m_handlerOnClick();
		if (!success)	// 객체가 파괴된 경우
			m_handlerOnClick = nullptr;
	}
}

const UIObjectHandle IUIObject::ToHandle() const
{
	assert(UIObjectManager::GetInstance()->m_handleTable[m_tableIndex] == this);

	return UIObjectHandle(m_tableIndex, m_id);
}

void IUIObject::UpdateActiveState(bool isParentActiveInHierarchy)
{
	RectTransform* pParentTransform = m_transform.m_pParent;
	
	// DFS 구조로 상위 계층 -> 하위 계층으로 내려가면서 업데이트하므로 부모의 IsActiveInHierarchy는 이미 업데이트되어있으므로 한 단계 부모의 플래그만 확인해도 된다.
	// bool activeInHierarchy = this->IsActiveSelf() && (pParentTransform ? pParentTransform->m_pUIObject->IsActiveInHierarchy() : true);
	bool activeInHierarchy = this->IsActiveSelf() && isParentActiveInHierarchy;

	if (this->IsActiveInHierarchy() == activeInHierarchy)
		return;

	if (activeInHierarchy)
	{
		this->OnFlag(UIOBJECT_FLAG::ACTIVE_IN_HIERARCHY);
		this->OnActivationSysJob();
	}
	else
	{
		this->OffFlag(UIOBJECT_FLAG::ACTIVE_IN_HIERARCHY);
		this->OnDeactivationSysJob();
	}

	for (RectTransform* pChildTransform : m_transform.m_children)
		pChildTransform->m_pUIObject->UpdateActiveState(activeInHierarchy);
}

void IUIObject::OnDeploySysJob()
{
	assert(this->IsPending());

	this->OffFlag(UIOBJECT_FLAG::PENDING);	// 중요!

	if (m_transform.GetParent() == nullptr)
		UIObjectManager::GetInstance()->AddToRootArray(this);

	if (this->IsActiveInHierarchy())
		UIObjectManager::GetInstance()->AddToActiveGroup(this);
	else
		UIObjectManager::GetInstance()->AddToInactiveGroup(this);
}

void IUIObject::OnActivationSysJob()
{
	if (this->IsPending())
		return;

	UIObjectManager::GetInstance()->MoveToActiveGroup(this);
}

void IUIObject::OnDeactivationSysJob()
{
	if (this->IsPending())
		return;

	UIObjectManager::GetInstance()->MoveToInactiveGroup(this);
}

constexpr float UI_SIZE_DEFAULT = 16.0f;

UISize::UISize()
	: m_size(UI_SIZE_DEFAULT, UI_SIZE_DEFAULT)
	, m_halfSize(m_size.x / 2.0f, m_size.y / 2.0f)
{
}

UISize::UISize(FLOAT width, FLOAT height)
	: m_size(width, height)
	, m_halfSize(m_size.x / 2.0f, m_size.y / 2.0f)
{
}

bool UISize::HitTest(POINT pt, POINT pos) const
{
	POINT ptL;	// pt in local space
	ptL.x = pt.x - pos.x;
	ptL.y = pt.y - pos.y;

	return std::abs(ptL.x) <= static_cast<LONG>(this->GetHalfSizeX()) && std::abs(ptL.y) <= static_cast<LONG>(this->GetHalfSizeY());
}

void XM_CALLCONV UISize::SetSize(FXMVECTOR size)
{
	XMStoreFloat2(&m_size, size);
	XMStoreFloat2(&m_halfSize, XMVectorMultiply(size, Vector3::OneHalf()));
}

void UISize::SetSize(FLOAT width, FLOAT height)
{
	m_size.x = width;
	m_size.y = height;

	m_halfSize.x = m_size.x / 2.0f;
	m_halfSize.y = m_size.y / 2.0f;
}

UIColor::UIColor()
	: m_color(ColorsLinear::LightGray)
{
}

UIColor::UIColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
	: m_color(r, g, b, a)
{
}

UIColor::UIColor(FXMVECTOR color)
{
	XMStoreFloat4(&m_color, color);
}

constexpr uint32_t DEFAULT_FONT_SIZE = 11;
constexpr uint32_t DEFAULT_MAX_CHAR = 64;

UIText::UIText()
	: m_text()
	, m_maxChar(DEFAULT_MAX_CHAR)
	, m_tf(L"", DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DEFAULT_FONT_SIZE)
	, m_textAlignment((DWRITE_TEXT_ALIGNMENT_LEADING))
	, m_paragraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
	, m_spDWriteTextFormatWrapper(GraphicDevice::GetInstance()->GetDWriteTextFormatWrapper(m_tf))
{
}

UIText::UIText(std::wstring text)
	: m_text(std::move(text))
	, m_maxChar(DEFAULT_MAX_CHAR)
	, m_tf(L"", DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DEFAULT_FONT_SIZE)
	, m_textAlignment((DWRITE_TEXT_ALIGNMENT_LEADING))
	, m_paragraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
	, m_spDWriteTextFormatWrapper(GraphicDevice::GetInstance()->GetDWriteTextFormatWrapper(m_tf))
{
}

void UIText::SetMaxChar(uint32_t count)
{
	m_maxChar = count;

	if (m_text.length() > count)
		m_text.resize(count);
}

void UIText::ApplyTextFormat()
{
	m_spDWriteTextFormatWrapper = GraphicDevice::GetInstance()->GetDWriteTextFormatWrapper(m_tf);
}
