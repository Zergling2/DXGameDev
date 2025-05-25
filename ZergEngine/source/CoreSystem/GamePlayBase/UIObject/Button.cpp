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
	// ���콺 �����Ϳ� �浹 �˻�

    XMVECTOR position = XMLoadFloat2(&m_transform.m_position);
    XMVECTOR halfSize = XMLoadFloat2(&m_halfSize);

	// ���콺 �����͸� UI�� ���� �����̽� �������� �̵���Ų �� �ٿ�� �˻縦 �Ѵ�
    XMVECTOR mousePosInButtonLocalSpace = XMVectorSubtract(XMLoadFloat2A(&mousePos), position);

	// XMVector2InBounds
	// (V.x <= Bounds.x && V.x >= -Bounds.x) && (V.y <= Bounds.y && V.y >= -Bounds.y);
	return XMVector2InBounds(mousePosInButtonLocalSpace, halfSize);
}
