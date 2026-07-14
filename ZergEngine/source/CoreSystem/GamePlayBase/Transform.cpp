#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>

using namespace ze;

Transform::Transform(GameObject* pGameObject)
    : m_pGameObject(pGameObject)
    , m_pParent(nullptr)
    , m_children()
{
	XMStoreFloat3A(&m_scale, XMVectorSplatOne());
	XMStoreFloat4A(&m_rotation, XMQuaternionIdentity());
	XMStoreFloat3A(&m_position, XMVectorZero());
}

XMMATRIX Transform::GetWorldTransformMatrix() const
{
    // SRT
    XMMATRIX localMatrix = XMMatrixScalingFromVector(XMLoadFloat3A(&m_scale)) * XMMatrixRotationQuaternion(XMLoadFloat4A(&m_rotation));
    localMatrix.r[3] = XMVectorSetW(XMLoadFloat3A(&m_position), 1.0f);


    if (m_pParent == nullptr)
        return localMatrix;     // 부모가 없으면 local == world
    else
        return localMatrix * m_pParent->GetWorldTransformMatrix();
}

void Transform::GetWorldTransform(XMFLOAT3* pScale, XMFLOAT4* pRotation, XMFLOAT3* pPosition) const
{
    assert(pScale != nullptr);
    assert(pRotation != nullptr);
    assert(pPosition != nullptr);

    XMVECTOR scale;
    XMVECTOR rotation;	// Quaternion
    XMVECTOR translation;
    this->GetWorldTransform(&scale, &rotation, &translation);

    XMStoreFloat3(pScale, scale);
    XMStoreFloat4(pRotation, rotation);
    XMStoreFloat3(pPosition, translation);
}

void Transform::GetWorldTransform(XMVECTOR* pScale, XMVECTOR* pRotation, XMVECTOR* pPosition) const
{
    XMMatrixDecompose(pScale, pRotation, pPosition, GetWorldTransformMatrix());
}

/*
###############DEPRECATED##############
###############DEPRECATED##############
###############DEPRECATED##############
void Transform::GetWorldTransform(XMVECTOR* pScale, XMVECTOR* pRotation, XMVECTOR* pPosition) const
{
    assert(pScale != nullptr);
    assert(pRotation != nullptr);
    assert(pPosition != nullptr);

    XMVECTOR scaleL = this->GetScale();
    XMVECTOR rotationL = this->GetRotation();
    XMVECTOR translationL = XMVectorSetW(this->GetPosition(), 1.0f);    // 매우 중요

    if (!m_pParent)
    {
        *pScale = scaleL;
        *pRotation = rotationL;
        *pPosition = translationL;
    }
    else
    {
        // 부모가 있는 경우
        
        // 중요
        // 로컬 Transform = 부모 공간에서의 상대적인 Transform
        // 따라서 로컬 Transform들은 부모에 의해 Transformation된다.
        // 
        // 자식의 Translation 성분은 부모의 스케일 및 회전에 영향을 받는다.
        // 자기 자신 로컬 스케일과 회전은 영향을 받지 않는다. (로컬 내에서의 Transform 성분 순서 규칙은 SRT이므로) -> (부모의 스케일 및 회전만 적용해야 함)

        XMVECTOR parentScaleW;
        XMVECTOR parentRotationW;
        XMVECTOR parentTranslationW;
        m_pParent->GetWorldTransform(&parentScaleW, &parentRotationW, &parentTranslationW);


        // 계층 구조에서 SRT 계산
        // -> 로컬 좌표계는 부모의 공간 기준으로 정의된 좌표계이다.
        // 이 로컬 좌표계는 부모의 월드 Transform 의해 변형된다.
        // 즉,
        // 1. S(local)                                      // 자기 자신의 공간에서의 스케일 값
        // 2. World Scale = S(local) * S(parent world)      // 자식의 로컬 스케일이 부모의 월드 스케일에 의해 변환됨.
        //                                                      (자식의 스케일에 영향을 미치는건 부모의 스케일뿐)
        XMVECTOR scaleW = XMVectorMultiply(scaleL, parentScaleW);


        // 1. Q(local)                                      // 자기 자신의 공간에서의 회전 값
        // 2. World Rotation = Q(local) * Q(parent world)   // 자식의 로컬 회전이 부모의 월드 회전에 의해 변환됨.
        //                                                      (자식의 회전에 영향을 미치는건 부모의 회전뿐)
        // v' = q * v * inv(q)
        // rotationW = parentRotationW(rotationL(v))
        XMVECTOR rotationW = XMQuaternionMultiply(rotationL, parentRotationW);  // rotationL 회전 먼저, parentRotationW 회전이 그 다음


        // 1. T(local)                                      // 자기 자신의 공간에서의 이동 값
        // 2. T(local) * S(parent world)                    // 자식의 로컬 이동이 부모의 스케일에 의해 변환됨.
        // 3. T(local) * S(parent world) * Q(parent world)  // 자식의 로컬 이동이 부모의 회전에 의해 변환됨.
        //                                                      (자식의 이동은 부모의 회전에 의해 변형됨)
        // 4. T(local) * S(parent world) * Q(parent world) + P(parent world)    // 마지막으로 부모의 월드 이동을 더해준다.



        // 1. 자식의 이동 성분은 부모의 스케일에 영향을 받는다.
        XMVECTOR scaledTranslationL = XMVectorMultiply(translationL, parentScaleW);
        // 2. 자식의 이동 성분은 부모의 회전에 영향을 받는다.
        XMVECTOR rotatedTranslationL = XMVector3Rotate(scaledTranslationL, parentRotationW);
        // 3. 계산된 자식의 Translation 성분은 부모의 월드 공간 기준 Translation 성분이다. 따라서 더해준다.
        XMVECTOR translationW = XMVectorAdd(rotatedTranslationL, parentTranslationW);

        *pScale = scaleW;
        *pRotation = rotationW;
        *pPosition = translationW;
    }
}
*/

XMVECTOR Transform::GetWorldScale() const
{
    XMVECTOR scaleW;
    XMVECTOR scaleL = this->GetScale();

    if (m_pParent)
        scaleW = XMVectorMultiply(scaleL, m_pParent->GetWorldScale());
    else
        scaleW = scaleL;

    return scaleW;
}

XMVECTOR Transform::GetWorldRotation() const
{
    XMVECTOR rotationW;
    XMVECTOR rotationL = this->GetRotation();

    if (m_pParent)
        rotationW = XMQuaternionMultiply(rotationL, m_pParent->GetWorldRotation()); // rotationL 회전이 먼저 이루어지고, 이후 parentWorldRotation 회전을 가한다.
    else
        rotationW = rotationL;

    return rotationW;
}

XMVECTOR Transform::GetWorldPosition() const
{
    XMVECTOR scaleW;
    XMVECTOR rotationW;
    XMVECTOR positionW;

    this->GetWorldTransform(&scaleW, &rotationW, &positionW);

    return positionW;
}

void XM_CALLCONV Transform::RotateQuaternion(FXMVECTOR quaternion)
{
    return XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), quaternion)));
}

void XM_CALLCONV Transform::RotateEuler(FXMVECTOR euler)
{
    return XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), XMQuaternionRotationRollPitchYawFromVector(euler))));
}

void Transform::SetWorldPosition(FLOAT x, FLOAT y, FLOAT z)
{
    // 1. Local transform matrix * Parent world transform matrix = World transform matrix
    // 
    // 2. Local transform matrix * Parent world transform matrix * Parent world transform matrix(Inv) = World transform matrix * Parent world transform matrix(Inv)
    // 
    // 3. 결합법칙에 따라 Local transform matrix = World transform matrix * Parent world transform matrix(Inv)

    if (m_pParent == nullptr)
    {
        m_position.x = x;
        m_position.y = y;
        m_position.z = z;
    }
    else
    {
        XMMATRIX invParentWorldMatrix = XMMatrixInverse(nullptr, m_pParent->GetWorldTransformMatrix());
        XMVECTOR localPosition = XMVector3TransformCoord(XMVectorSet(x, y, z, 1.0f), invParentWorldMatrix);

        XMStoreFloat3A(&m_position, localPosition);
    }
}

bool Transform::SetParent(Transform* pTransform)
{
    return GameObjectManager::GetInstance()->SetParent(this, pTransform);
}

bool Transform::IsDescendantOf(Transform* pTransform) const
{
    const Transform* pCursor = m_pParent;

    while (pCursor != nullptr)
    {
        if (pCursor == pTransform)
            return true;

        pCursor = pCursor->m_pParent;
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
