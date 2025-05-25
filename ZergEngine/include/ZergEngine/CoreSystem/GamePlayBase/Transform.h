#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class GameObject;
	
	class Transform
	{
		friend class RuntimeImpl;
		friend class GameObjectManagerImpl;
		friend class GameObject;
	public:
		Transform(GameObject* pGameObject) noexcept;
		Transform(const Transform&) = delete;
		Transform(Transform&&) = delete;
		~Transform() = default;
		Transform& operator=(const Transform&) = delete;

		XMMATRIX GetWorldTransformMatrix() const noexcept;
		XMMATRIX GetLocalTransformMatrix() const noexcept
		{
			return 
				XMMatrixScalingFromVector(XMLoadFloat3A(&m_scale)) *
				XMMatrixRotationQuaternion(XMLoadFloat4A(&m_rotation)) *
				XMMatrixTranslationFromVector(XMLoadFloat3A(&m_position));
		}

		XMVECTOR XM_CALLCONV GetLocalScale() const { return XMLoadFloat3A(&m_scale); }

		// ȸ�� ���¸� ��Ÿ���� ���ʹϾ��� ��ȯ�մϴ�.
		XMVECTOR XM_CALLCONV GetLocalRotation() const { return XMLoadFloat4A(&m_rotation); }
		XMVECTOR XM_CALLCONV GetWorldRotation() const;

		XMVECTOR XM_CALLCONV GetLocalPosition() const { return XMLoadFloat3A(&m_position); }
		XMVECTOR XM_CALLCONV GetWorldPosition() const;

		// �������� �����մϴ�.
		void XM_CALLCONV Scale(FXMVECTOR scale) { XMStoreFloat3A(&m_scale, XMVectorMultiply(XMLoadFloat3A(&m_scale), scale)); }
		void Scale(const XMFLOAT3& scale) { XMStoreFloat3A(&m_scale, XMVectorMultiply(XMLoadFloat3A(&m_scale), XMLoadFloat3(&scale))); }
		void Scale(const XMFLOAT3A& scale) { XMStoreFloat3A(&m_scale, XMVectorMultiply(XMLoadFloat3A(&m_scale), XMLoadFloat3A(&scale))); }
		void XM_CALLCONV SetScale(FXMVECTOR scale) { XMStoreFloat3A(&m_scale, scale); }
		void SetScale(const XMFLOAT3& scale) { SetScale(XMLoadFloat3(&scale)); }
		void SetScale(const XMFLOAT3A& scale) { m_scale = scale; }

		// ������ ���¿��� �߰������� ȸ����ŵ�ϴ�.
		void XM_CALLCONV Rotate(FXMVECTOR quaternion) { XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), quaternion))); }
		void Rotate(const XMFLOAT3& euler) { XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z)))); }
		void Rotate(const XMFLOAT3A& euler) { XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3A(&euler))))); }

		// ȸ�������� �缳���մϴ�.
		// ���Ϸ� ������ ��� ��ȯ ������ Z(Roll), X(Pitch), Y(Yaw)�Դϴ�. ȸ���� ������ �����Դϴ�.
		void XM_CALLCONV SetRotation(FXMVECTOR quaternion) { XMStoreFloat4A(&m_rotation, quaternion); }
		void SetRotation(const XMFLOAT3& euler) { XMStoreFloat4A(&m_rotation, XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z)); }
		void SetRotation(const XMFLOAT3A& euler) { XMStoreFloat4A(&m_rotation, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3A(&euler))); }

		// Rotates the transform about axis passing through point in world coordinates by angle radians.
		// point: ���� ��ǥ�� ���� ȸ�� �߽���
		// axis: ���� ��ǥ�� ���� ȸ�� ��
		// angle: ȸ���� ���� (���� ����)
		void XM_CALLCONV RotateAround(FXMVECTOR point, FXMVECTOR axis, FLOAT angle);

		void XM_CALLCONV Translate(FXMVECTOR translation) { XMStoreFloat3A(&m_position, XMVectorAdd(XMLoadFloat3A(&m_position), translation)); }
		void XM_CALLCONV SetPosition(FXMVECTOR position) { XMStoreFloat3A(&m_position, position); }
		void SetPosition(const XMFLOAT3& position) { SetPosition(XMLoadFloat3(&position)); }
		void SetPosition(const XMFLOAT3A& position) { m_position = position; }
		void XM_CALLCONV SetWorldPosition(FXMVECTOR position);
		void SetWorldPosition(const XMFLOAT3& position) { SetWorldPosition(XMLoadFloat3(&position)); }
		void SetWorldPosition(const XMFLOAT3A& position) { SetWorldPosition(XMLoadFloat3A(&position)); }

		bool SetParent(Transform* pTransform);
		bool IsDescendantOf(Transform* pTransform) const;

		GameObjectHandle GetChild(uint32_t index);
	private:
		GameObject* m_pGameObject;
		Transform* m_pParentTransform;
		std::vector<Transform*> m_children;

		XMFLOAT3A m_scale;
		XMFLOAT4A m_rotation;	// Quaternion
		XMFLOAT3A m_position;
	};
}
