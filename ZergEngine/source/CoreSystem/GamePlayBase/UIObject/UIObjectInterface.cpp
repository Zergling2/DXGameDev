#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
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
	// 자식 오브젝트들 재귀적 활성화
	for (RectTransform* pChildTransform : m_transform.m_children)
		pChildTransform->m_pUIObject->SetActive(active);

	UIObjectManager::GetInstance()->SetActive(this, active);
}

const UIObjectHandle IUIObject::ToHandle() const
{
	assert(UIObjectManager::GetInstance()->m_handleTable[m_tableIndex] == this);

	return UIObjectHandle(m_tableIndex, m_id);
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

	return std::abs(ptL.x) < static_cast<LONG>(this->GetHalfSizeX()) && std::abs(ptL.y) < static_cast<LONG>(this->GetHalfSizeY());
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

UIText::UIText()
	: m_text()
	, m_tf(L"", DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DEFAULT_FONT_SIZE)
	, m_textAlignment((DWRITE_TEXT_ALIGNMENT_LEADING))
	, m_paragraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
	, m_spDWriteTextFormatWrapper(GraphicDevice::GetInstance()->GetDWriteTextFormatWrapper(m_tf))
{
}

UIText::UIText(std::wstring text)
	: m_text(std::move(text))
	, m_tf(L"", DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DEFAULT_FONT_SIZE)
	, m_textAlignment((DWRITE_TEXT_ALIGNMENT_LEADING))
	, m_paragraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
	, m_spDWriteTextFormatWrapper(GraphicDevice::GetInstance()->GetDWriteTextFormatWrapper(m_tf))
{
}

void UIText::ApplyTextFormat()
{
	m_spDWriteTextFormatWrapper = GraphicDevice::GetInstance()->GetDWriteTextFormatWrapper(m_tf);
}
