#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

XMVECTOR RectTransform::GetHCSPosition() const
{
    XMFLOAT2A basis;

    switch (m_ha)
    {
    case HorizontalAnchor::Left:
        basis.x = -GraphicDevice::GetInstance()->GetSwapChainHalfWidthFlt();
        break;
    case HorizontalAnchor::Center:
        basis.x = 0.0f;
        break;
    case HorizontalAnchor::Right:
        basis.x = +GraphicDevice::GetInstance()->GetSwapChainHalfWidthFlt();
        break;
    }

    switch (m_va)
    {
    case VerticalAnchor::Top:
        basis.y = +GraphicDevice::GetInstance()->GetSwapChainHalfHeightFlt();
        break;
    case VerticalAnchor::VCenter:
        basis.y = 0.0f;
        break;
    case VerticalAnchor::Bottom:
        basis.y = -GraphicDevice::GetInstance()->GetSwapChainHalfHeightFlt();
        break;
    }

    return XMVectorAdd(XMLoadFloat2A(&basis), XMLoadFloat2(&m_position));
}

XMVECTOR RectTransform::GetScreenPosWindowsCoordSystem() const
{
    XMFLOAT2A basis;
    XMFLOAT2A windowsPosition(m_position.x, -m_position.y);

    switch (m_ha)
    {
    case HorizontalAnchor::Left:
        basis.x = 0.0f;
        break;
    case HorizontalAnchor::Center:
        basis.x = GraphicDevice::GetInstance()->GetSwapChainHalfWidthFlt();
        break;
    case HorizontalAnchor::Right:
        basis.x = GraphicDevice::GetInstance()->GetSwapChainWidthFlt();
        break;
    }

    switch (m_va)
    {
    case VerticalAnchor::Top:
        basis.y = 0.0f;
        break;
    case VerticalAnchor::VCenter:
        basis.y = GraphicDevice::GetInstance()->GetSwapChainHalfHeightFlt();
        break;
    case VerticalAnchor::Bottom:
        basis.y = GraphicDevice::GetInstance()->GetSwapChainHeightFlt();
        break;
    }

    return XMVectorAdd(XMLoadFloat2A(&basis), XMLoadFloat2A(&windowsPosition));
}

bool RectTransform::SetParent(RectTransform* pTransform)
{
    return UIObjectManager::GetInstance()->SetParent(this, pTransform);
}

bool RectTransform::IsDescendantOf(RectTransform* pTransform) const
{
    const RectTransform* pCursor = m_pParentTransform;

    while (pCursor != nullptr)
    {
        if (pCursor == pTransform)
            return true;

        pCursor = pCursor->m_pParentTransform;
    }

    return false;
}
