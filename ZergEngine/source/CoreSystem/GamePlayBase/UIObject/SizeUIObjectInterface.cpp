#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeUIObjectInterface.h>
#include <ZergEngine\Common\EngineConstants.h>

using namespace ze;

constexpr uint16_t MIN_UI_WIDTH = 16;
constexpr uint16_t MIN_UI_HEIGHT = 16;

ISizeUIObject::ISizeUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name, UIOBJECT_TYPE type)
	: IUIObject(id, flag, name, type)
	, m_size(static_cast<FLOAT>(MIN_UI_WIDTH), static_cast<FLOAT>(MIN_UI_HEIGHT))
	, m_halfSize(m_size.x * 0.5f, m_size.y * 0.5f)
{
}

void XM_CALLCONV ISizeUIObject::SetSize(FXMVECTOR size)
{
	XMStoreFloat2(&m_size, size);
	XMStoreFloat2(&m_halfSize, XMVectorMultiply(size, g_XMOneHalf));
}

bool XM_CALLCONV ISizeUIObject::HitTest(FXMVECTOR mousePosition) const
{
	// ����� �����ϸ� ���� ��忡���� ����!!!
	// �����ϸ��� �Ͼ�� ��üȭ�� ��忡���� ���� ����� ���� X

	// ���콺 �����͸� UI�� ���� �����̽� �������� �̵���Ų �� �ιٿ�� �˻縦 �Ѵ�
	XMVECTOR mousePosInButtonLocalSpace = XMVectorSubtract(mousePosition, m_transform.GetWindowsScreenPosition());

	XMVECTOR bound = XMLoadFloat2(&m_halfSize);

	// XMVector2InBounds
	// (V.x <= Bounds.x && V.x >= -Bounds.x) && (V.y <= Bounds.y && V.y >= -Bounds.y);
	return XMVector2InBounds(mousePosInButtonLocalSpace, bound);
}
