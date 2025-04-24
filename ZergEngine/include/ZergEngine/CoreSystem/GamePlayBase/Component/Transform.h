#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class Transform : public IComponent
	{
		friend class GameObject;
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::TRANSFORM;
		static bool IsCreatable() { return false; }

		Transform() noexcept;
		virtual ~Transform() = default;

		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::TRANSFORM; }

		XMVECTOR XM_CALLCONV GetScale() const { return XMLoadFloat3A(&m_scale); }

		// ȸ�� ���¸� ��Ÿ���� ���ʹϾ��� ��ȯ�մϴ�.
		XMVECTOR XM_CALLCONV GetRotation() const { return XMLoadFloat4A(&m_rotation); }

		XMVECTOR XM_CALLCONV GetPosition() const { return XMLoadFloat3A(&m_position); }

		// �������� �����մϴ�.
		void XM_CALLCONV Scale(XMVECTOR scale) { XMStoreFloat3A(&m_scale, XMLoadFloat3A(&m_scale) * scale); }
		void Scale(const XMFLOAT3& scale) { XMStoreFloat3A(&m_scale, XMLoadFloat3A(&m_scale) * XMLoadFloat3(&scale)); }
		void Scale(const XMFLOAT3A& scale) { XMStoreFloat3A(&m_scale, XMLoadFloat3A(&m_scale) * XMLoadFloat3A(&scale)); }
		void XM_CALLCONV SetScale(XMVECTOR scale) { XMStoreFloat3A(&m_scale, scale); }
		void SetScale(const XMFLOAT3& scale) { SetScale(XMLoadFloat3(&scale)); }
		void SetScale(const XMFLOAT3A& scale) { m_scale = scale; }

		// ������ ���¿��� �߰������� ȸ����ŵ�ϴ�.
		void XM_CALLCONV Rotate(XMVECTOR quaternion) { XMStoreFloat4A(&m_rotation, XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), quaternion)); }
		void Rotate(const XMFLOAT3& euler) { XMStoreFloat4A(&m_rotation, XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z))); }
		void Rotate(const XMFLOAT3A& euler) { XMStoreFloat4A(&m_rotation, XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3A(&euler)))); }

		// ȸ�������� �缳���մϴ�.
		// ���Ϸ� ������ ��� ��ȯ ������ Z(Roll), X(Pitch), Y(Yaw)�Դϴ�. ȸ���� ������ �����Դϴ�.
		void XM_CALLCONV SetRotation(XMVECTOR quaternion) { XMStoreFloat4A(&m_rotation, quaternion); }
		void SetRotation(const XMFLOAT3& euler) { XMStoreFloat4A(&m_rotation, XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z)); }
		void SetRotation(const XMFLOAT3A& euler) { XMStoreFloat4A(&m_rotation, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3A(&euler))); }

		void XM_CALLCONV Translate(XMVECTOR translation) { XMStoreFloat3A(&m_position, XMLoadFloat3A(&m_position) + translation); }
		void XM_CALLCONV SetPosition(XMVECTOR position) { XMStoreFloat3A(&m_position, position); }
		void SetPosition(const XMFLOAT3& position) { SetPosition(XMLoadFloat3(&position)); }
		void SetPosition(const XMFLOAT3A& position) { m_position = position; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
		XMFLOAT3A m_scale;
		XMFLOAT4A m_rotation;	// Quaternion
		XMFLOAT3A m_position;
	};
}
