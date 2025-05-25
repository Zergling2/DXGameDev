#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

using namespace ze;

constexpr uint16_t BUTTON_MIN_WIDTH = 16;
constexpr uint16_t BUTTON_MIN_HEIGHT = 16;

Button::Button(UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(flag, name, UIObjectManager.AssignUniqueId(), UIOBJECT_TYPE::BUTTON)
	, m_pressed(false)
	, m_size(BUTTON_MIN_WIDTH, BUTTON_MIN_HEIGHT)
	, m_halfSize(m_size.x * 0.5f, m_size.y * 0.5f)
	, m_color(0.75f, 0.75f, 0.75f, 1.0f)
{
}

void XM_CALLCONV Button::SetSize(FXMVECTOR size)
{
	XMStoreFloat2(&m_size, size);
	XMStoreFloat2(&m_halfSize, XMVectorMultiply(size, XMVectorReplicate(0.5f)));
}

bool Button::HitTest(const XMFLOAT2A mousePos) const
{
	// 마우스 포인터와 충돌 검사

    XMVECTOR position = XMLoadFloat2(&m_transform.m_position);
    XMVECTOR halfSize = XMLoadFloat2(&m_halfSize);

	// 마우스 포인터를 UI의 로컬 스페이스 공간으로 이동시킨 뒤 바운드 검사를 한다
    XMVECTOR mousePosInButtonLocalSpace = XMVectorSubtract(XMLoadFloat2A(&mousePos), position);

	// XMVector2InBounds
	// (V.x <= Bounds.x && V.x >= -Bounds.x) && (V.y <= Bounds.y && V.y >= -Bounds.y);
	return XMVector2InBounds(mousePosInButtonLocalSpace, halfSize);
}
