#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class GameObject;
	
	class Transform
	{
		friend class GameObjectManager;
		friend class GameObject;
	public:
		Transform(GameObject* pGameObject);
		Transform(const Transform&) = delete;
		Transform(Transform&&) = delete;
		~Transform() = default;
		Transform& operator=(const Transform&) = delete;

		XMMATRIX GetWorldTransformMatrix() const;

		// ���� ���� �������� ��ȯ�մϴ�.
		XMVECTOR GetScale() const { return XMLoadFloat3A(&m_scale); }

		// ���� ���� �������� ��ȯ�մϴ�.
		XMVECTOR GetWorldScale() const;

		// ���� ���� ȸ�� ���¸� ��Ÿ���� ���ʹϾ��� ��ȯ�մϴ�.
		XMVECTOR GetRotation() const { return XMLoadFloat4A(&m_rotation); }

		// ���� ���� ȸ�� ���¸� ��Ÿ���� ���ʹϾ��� ��ȯ�մϴ�.
		XMVECTOR GetWorldRotation() const;

		// ���� ���� ��ġ�� ��ȯ�մϴ�.
		XMVECTOR GetPosition() const { return XMLoadFloat3A(&m_position); }

		// ���� ���� ��ġ�� ��ȯ�մϴ�.
		XMVECTOR GetWorldPosition() const;

		// ���� ������ ���¿� �����ϸ��� �����մϴ�.
		void XM_CALLCONV Scale(FLOAT scale) { XMStoreFloat3A(&m_scale, XMVectorScale(XMLoadFloat3A(&m_scale), scale)); }

		// ���� ������ ���¿� �����ϸ��� �����մϴ�.
		void XM_CALLCONV Scale(FXMVECTOR scale) { XMStoreFloat3A(&m_scale, XMVectorMultiply(XMLoadFloat3A(&m_scale), scale)); }

		// ���� �������� �缳���մϴ�.
		void XM_CALLCONV SetScale(FXMVECTOR scale) { XMStoreFloat3A(&m_scale, scale); }

		// ���� ���¿��� �߰������� ȸ����ŵ�ϴ�.
		void XM_CALLCONV RotateQuaternion(FXMVECTOR quaternion) { XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), quaternion))); }

		// ���� ���¿��� �߰������� ȸ����ŵ�ϴ�.
		void XM_CALLCONV RotateEuler(FXMVECTOR euler) { XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), XMQuaternionRotationRollPitchYawFromVector(euler)))); }

		// ���� ȸ�������� ���ʹϾ����� �缳���մϴ�.
		void XM_CALLCONV SetRotationQuaternion(FXMVECTOR quaternion) { XMStoreFloat4A(&m_rotation, quaternion); }

		// ���� ȸ�������� ���Ϸ� ������ �缳���մϴ�.
		// ��ȯ ������ Z(Roll), X(Pitch), Y(Yaw)�Դϴ�. ȸ���� ������ �����Դϴ�.
		void XM_CALLCONV SetRotationEuler(FXMVECTOR euler) { SetRotationQuaternion(XMQuaternionRotationRollPitchYawFromVector(euler)); }

		// Rotates the transform about axis passing through point in world coordinates by angle radians.
		// point: ���� ��ǥ���� ȸ�� �߽���
		// axis: ���� ��ǥ���� ȸ�� ��
		// angle: ȸ���� ���� (���� ����)
		void XM_CALLCONV RotateAround(FXMVECTOR point, FXMVECTOR axis, FLOAT angle);

		void XM_CALLCONV Translate(FXMVECTOR translation) { XMStoreFloat3A(&m_position, XMVectorAdd(XMLoadFloat3A(&m_position), translation)); }

		void XM_CALLCONV SetPosition(FXMVECTOR position) { XMStoreFloat3A(&m_position, position); }

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
