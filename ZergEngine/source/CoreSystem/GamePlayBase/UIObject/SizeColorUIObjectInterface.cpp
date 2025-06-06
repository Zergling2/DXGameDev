#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>

using namespace ze;

constexpr uint16_t BUTTON_MIN_WIDTH = 16;
constexpr uint16_t BUTTON_MIN_HEIGHT = 16;

ISizeColorUIObject::ISizeColorUIObject(UIOBJECT_FLAG flag, PCWSTR name, UIOBJECT_TYPE type)
	: IUIObject(flag, name, type)
	, m_size(BUTTON_MIN_WIDTH, BUTTON_MIN_HEIGHT)
	, m_halfSize(m_size.x * 0.5f, m_size.y * 0.5f)
	, m_color(0.75f, 0.75f, 0.75f, 1.0f)
{
}

void XM_CALLCONV ISizeColorUIObject::SetSize(FXMVECTOR size)
{
	XMStoreFloat2(&m_size, size);
	XMStoreFloat2(&m_halfSize, XMVectorMultiply(size, XMVectorReplicate(0.5f)));
}

bool XM_CALLCONV ISizeColorUIObject::HitTest(FXMVECTOR mousePosition) const
{
	// ����� �����ϸ� ���� ��忡���� ����!!!
	// �����ϸ��� �Ͼ�� ��üȭ�� ��忡���� ���� ����� ���� X

	// ���콺 �����͸� UI�� ���� �����̽� �������� �̵���Ų �� �ιٿ�� �˻縦 �Ѵ�
	XMVECTOR mousePosInButtonLocalSpace = XMVectorSubtract(mousePosition, m_transform.GetUnityScreenPosition());

	XMVECTOR bound = XMLoadFloat2(&m_halfSize);

	// XMVector2InBounds
	// (V.x <= Bounds.x && V.x >= -Bounds.x) && (V.y <= Bounds.y && V.y >= -Bounds.y);
	return XMVector2InBounds(mousePosInButtonLocalSpace, bound);
}