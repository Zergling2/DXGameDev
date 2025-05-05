#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TransformManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

Transform::Transform() noexcept
	: IComponent(TransformManager.AssignUniqueId())
    , m_pParentTransform(nullptr)
    , m_children()
{
	XMStoreFloat3A(&m_scale, XMVectorSplatOne());
	XMStoreFloat4A(&m_rotation, XMQuaternionIdentity());
	XMStoreFloat3A(&m_position, XMVectorZero());
}

XMMATRIX Transform::GetWorldTransformMatrix() const noexcept
{
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
        return m_pParentTransform->GetWorldTransformMatrix() * this->GetLocalTransformMatrix();
    else
        return this->GetLocalTransformMatrix();
}

XMVECTOR XM_CALLCONV Transform::GetWorldRotation() const
{
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
        return XMQuaternionMultiply(m_pParentTransform->GetWorldRotation(), this->GetLocalRotation());
    else
        return this->GetLocalRotation();
}

XMVECTOR XM_CALLCONV Transform::GetWorldPosition() const
{
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
        return this->GetWorldTransformMatrix().r[3];
    else
        return this->GetLocalPosition();
}

void XM_CALLCONV Transform::RotateAround(XMVECTOR point, XMVECTOR axis, FLOAT angle)
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
        Transform* pParentTransform = m_pParentTransform;

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

bool Transform::SetParent(Transform* pTransform)
{
    // �ڱ� �ڽ��� �θ�� �����Ϸ��� ���
    if (pTransform == this)
        return false;

    // �̹� �θ��� ���
    if (m_pParentTransform == pTransform)
        return false;

    // Check cycle
    // pTransform�� �̹� ���� �������� �ϰ� �ִ� ���
    if (pTransform != nullptr && pTransform->IsDescendantOf(this))
        return false;

    // ���� �θ𿡼� ������ �ڽ� �����͸� ã�� ����
    if (m_pParentTransform != nullptr)
    {
#if defined(DEBUG) || defined(_DEBUG)
        bool find = false;
#endif
        std::vector<Transform*>::const_iterator end = m_pParentTransform->m_children.cend();
        std::vector<Transform*>::const_iterator iter = m_pParentTransform->m_children.cbegin();
        while (iter != end)
        {
            if ((*iter) == this)
            {
                m_pParentTransform->m_children.erase(iter);
#if defined(DEBUG) || defined(_DEBUG)
                find = true;
#endif
                break;
            }
        }
        assert(find == true);
    }

    // �θ��� �ڽ� ����� ������Ʈ
    if (pTransform != nullptr)
        pTransform->m_children.push_back(this);

    // �ڽ��� �θ� �����͸� ������Ʈ
    m_pParentTransform = pTransform;

    return true;
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

IComponentManager* Transform::GetComponentManager() const
{
	return &TransformManager;
}
