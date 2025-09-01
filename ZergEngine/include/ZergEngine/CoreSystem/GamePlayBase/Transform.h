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

		// 현재 로컬 스케일을 반환합니다.
		XMVECTOR GetScale() const { return XMLoadFloat3A(&m_scale); }

		// 현재 월드 스케일을 반환합니다.
		XMVECTOR GetWorldScale() const;

		// 현재 로컬 회전 상태를 나타내는 쿼터니언을 반환합니다.
		XMVECTOR GetRotation() const { return XMLoadFloat4A(&m_rotation); }

		// 현재 월드 회전 상태를 나타내는 쿼터니언을 반환합니다.
		XMVECTOR GetWorldRotation() const;

		// 현재 로컬 위치를 반환합니다.
		XMVECTOR GetPosition() const { return XMLoadFloat3A(&m_position); }

		// 현재 월드 위치를 반환합니다.
		XMVECTOR GetWorldPosition() const;

		// 현재 스케일 상태에 스케일링을 누적합니다.
		void XM_CALLCONV Scale(FLOAT scale) { XMStoreFloat3A(&m_scale, XMVectorScale(XMLoadFloat3A(&m_scale), scale)); }

		// 현재 스케일 상태에 스케일링을 누적합니다.
		void XM_CALLCONV Scale(FXMVECTOR scale) { XMStoreFloat3A(&m_scale, XMVectorMultiply(XMLoadFloat3A(&m_scale), scale)); }

		// 로컬 스케일을 재설정합니다.
		void XM_CALLCONV SetScale(FXMVECTOR scale) { XMStoreFloat3A(&m_scale, scale); }

		// 현재 상태에서 추가적으로 회전시킵니다.
		void XM_CALLCONV RotateQuaternion(FXMVECTOR quaternion) { XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), quaternion))); }

		// 현재 상태에서 추가적으로 회전시킵니다.
		void XM_CALLCONV RotateEuler(FXMVECTOR euler) { XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(XMQuaternionMultiply(XMLoadFloat4A(&m_rotation), XMQuaternionRotationRollPitchYawFromVector(euler)))); }

		// 로컬 회전각도를 쿼터니언으로 재설정합니다.
		void XM_CALLCONV SetRotationQuaternion(FXMVECTOR quaternion) { XMStoreFloat4A(&m_rotation, quaternion); }

		// 로컬 회전각도를 오일러 각도로 재설정합니다.
		// 변환 순서는 Z(Roll), X(Pitch), Y(Yaw)입니다. 회전각 단위는 라디안입니다.
		void XM_CALLCONV SetRotationEuler(FXMVECTOR euler) { SetRotationQuaternion(XMQuaternionRotationRollPitchYawFromVector(euler)); }

		// Rotates the transform about axis passing through point in world coordinates by angle radians.
		// point: 월드 좌표계의 회전 중심점
		// axis: 월드 좌표계의 회전 축
		// angle: 회전할 각도 (라디안 단위)
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
