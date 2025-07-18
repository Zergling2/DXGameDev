#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

XMVECTOR XM_CALLCONV RectTransform::GetPreNDCPosition() const
{
    XMFLOAT2A basis;

    switch (m_ha)
    {
    case HORIZONTAL_ANCHOR::LEFT:
        basis.x = -GraphicDevice::GetInstance()->GetSwapChainHalfWidthFlt();
        break;
    case HORIZONTAL_ANCHOR::CENTER:
        basis.x = 0.0f;
        break;
    case HORIZONTAL_ANCHOR::RIGHT:
        basis.x = +GraphicDevice::GetInstance()->GetSwapChainHalfWidthFlt();
        break;
    }

    switch (m_va)
    {
    case VERTICAL_ANCHOR::TOP:
        basis.y = +GraphicDevice::GetInstance()->GetSwapChainHalfHeightFlt();
        break;
    case VERTICAL_ANCHOR::VCENTER:
        basis.y = 0.0f;
        break;
    case VERTICAL_ANCHOR::BOTTOM:
        basis.y = -GraphicDevice::GetInstance()->GetSwapChainHalfHeightFlt();
        break;
    }

    return XMVectorAdd(XMLoadFloat2A(&basis), XMLoadFloat2(&m_position));
}

XMVECTOR XM_CALLCONV RectTransform::GetWindowsScreenPosition() const
{
    XMFLOAT2A basis;
    XMFLOAT2A windowsPosition(m_position.x, -m_position.y);

    switch (m_ha)
    {
    case HORIZONTAL_ANCHOR::LEFT:
        basis.x = 0.0f;
        break;
    case HORIZONTAL_ANCHOR::CENTER:
        basis.x = GraphicDevice::GetInstance()->GetSwapChainHalfWidthFlt();
        break;
    case HORIZONTAL_ANCHOR::RIGHT:
        basis.x = GraphicDevice::GetInstance()->GetSwapChainWidthFlt();
        break;
    }

    switch (m_va)
    {
    case VERTICAL_ANCHOR::TOP:
        basis.y = 0.0f;
        break;
    case VERTICAL_ANCHOR::VCENTER:
        basis.y = GraphicDevice::GetInstance()->GetSwapChainHalfHeightFlt();
        break;
    case VERTICAL_ANCHOR::BOTTOM:
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
