#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>

using namespace ze;

Transform::Transform(GameObject* pGameObject)
    : m_pGameObject(pGameObject)
    , m_pParentTransform(nullptr)
    , m_children()
{
	XMStoreFloat3A(&m_scale, XMVectorSplatOne());
	XMStoreFloat4A(&m_rotation, XMQuaternionIdentity());
	XMStoreFloat3A(&m_position, XMVectorZero());
}

XMMATRIX Transform::GetWorldTransformMatrix() const
{
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
        return m_pParentTransform->GetWorldTransformMatrix() * this->GetLocalTransformMatrix();
    else
        return this->GetLocalTransformMatrix();
}

XMVECTOR Transform::GetWorldRotation() const
{
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
        return XMQuaternionMultiply(m_pParentTransform->GetWorldRotation(), this->GetLocalRotation());
    else
        return this->GetLocalRotation();
}

XMVECTOR Transform::GetWorldPosition() const
{
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
        return this->GetWorldTransformMatrix().r[3];
    else
        return this->GetLocalPosition();
}

void XM_CALLCONV Transform::RotateAround(FXMVECTOR point, FXMVECTOR axis, FLOAT angle)
{
    // 1. ȸ�� ���� ���ο� world position ���
    // ���� Transform�� ���� ��ġ�� ȸ�� ���
    XMVECTOR oldWorldPos = GetWorldPosition();

    // point ���� axis�� ȸ�������� ȸ��
    XMVECTOR offset = oldWorldPos - point;

    // ȸ�� ��� ����
    XMMATRIX rotationMatrix = XMMatrixRotationAxis(axis, angle);

    // ������ ȸ��
    XMVECTOR rotatedOffset = XMVector3Transform(offset, rotationMatrix);

    // ���ο� world position ���
    XMVECTOR newWorldPosition = point + rotatedOffset;

    // 2. ȸ�� ���� ���ο� world rotation ���
    // ������ world ���� ȸ�� ���¿� RotateAround �Լ��� ���� �߰����� ȸ���� ������Ű�� �ȴ�.
    
    // ���� world rotation ��� (worldMatrix���� �����ϰų�, ���� ���)
    XMVECTOR oldWorldRotation = this->GetWorldRotation();

    // ȸ���� �������� ȸ���ϴ� ���ʹϾ� ����
    XMVECTOR deltaRotation = XMQuaternionRotationAxis(axis, angle);
    
    // ���ο� world rotation ���
    XMVECTOR newWorldRotation = XMQuaternionMultiply(deltaRotation, oldWorldRotation);

    // 4. newWorldPos, newWorldRotation�� newLocalPos, newLocalRotation���� ��ȯ
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
    {
        const Transform* pParentTransform = m_pParentTransform;

        // �θ��� ���� ��Ʈ����
        XMMATRIX parentWorldMatrix = pParentTransform->GetWorldTransformMatrix();

        // �θ��� ���� ȸ�� (���ʹϾ�)
        XMVECTOR parentWorldRotation = pParentTransform->GetWorldRotation();

        // �θ� space �������� ��ȯ
        // ��ġ ��ȯ
        XMMATRIX parentWorldMatrixInv = XMMatrixInverse(nullptr, parentWorldMatrix);
        XMVECTOR newLocalPosition = XMVector3TransformCoord(newWorldPosition, parentWorldMatrixInv);
        
        // ȸ�� ��ȯ
        XMVECTOR parentWorldRotationInv = XMQuaternionInverse(parentWorldRotation);
        XMVECTOR newLocalRotation = XMQuaternionMultiply(newWorldRotation, parentWorldRotationInv);

        XMStoreFloat3A(&m_position, newLocalPosition);
        XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(newLocalRotation));
    }
    else
    {
        // �θ� ������Ʈ�� ���ٸ� world == local
        XMStoreFloat3A(&m_position, newWorldPosition);
        XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(newWorldRotation));
    }
}

void XM_CALLCONV Transform::SetWorldPosition(FXMVECTOR position)
{
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
    {
        const Transform* pParentTransform = m_pParentTransform;

        // �θ��� ���� ��Ʈ����
        XMMATRIX parentWorldMatrix = pParentTransform->GetWorldTransformMatrix();

        // �θ� space �������� ��ȯ
        // ��ġ ��ȯ
        XMMATRIX parentWorldMatrixInv = XMMatrixInverse(nullptr, parentWorldMatrix);
        XMVECTOR newLocalPosition = XMVector3TransformCoord(position, parentWorldMatrixInv);

        XMStoreFloat3A(&m_position, newLocalPosition);
    }
    else
    {
        // �θ� ������Ʈ�� ���ٸ� world == local
        XMStoreFloat3A(&m_position, position);
    }
}

bool Transform::SetParent(Transform* pTransform)
{
    return GameObjectManager::GetInstance()->SetParent(this, pTransform);
}

bool Transform::IsDescendantOf(Transform* pTransform) const
{
    const Transform* pCursor = m_pParentTransform;

    while (pCursor != nullptr)
    {
        if (pCursor == pTransform)
            return true;

        pCursor = pCursor->m_pParentTransform;
    }

    return false;
}

GameObjectHandle Transform::GetChild(uint32_t index)
{
    if (index >= static_cast<uint32_t>(m_children.size()))
        return GameObjectHandle();
    else
        return m_children[index]->m_pGameObject->ToHandle();
}
