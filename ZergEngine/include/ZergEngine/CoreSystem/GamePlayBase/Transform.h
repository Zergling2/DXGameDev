#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <ZergEngine\CoreSystem\Platform.h>
#include <vector>

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

		// pScale: 월드 스케일이 반환됩니다.
		// pRotation: 월드 회전이 쿼터니언으로 반환됩니다.
		// pPosition: 월드 위치가 반환됩니다.
		void GetWorldTransform(XMFLOAT3* pScale, XMFLOAT4* pRotation, XMFLOAT3* pPosition) const;

		// pScale: 월드 스케일이 반환됩니다.
		// pRotation: 월드 회전이 쿼터니언으로 반환됩니다.
		// pPosition: 월드 위치가 반환됩니다.
		void GetWorldTransform(XMFLOAT3A* pScale, XMFLOAT4A* pRotation, XMFLOAT3A* pPosition) const;

		// pScale: 월드 스케일이 반환됩니다.
		// pRotation: 월드 회전이 쿼터니언으로 반환됩니다.
		// pPosition: 월드 위치가 반환됩니다.
		void GetWorldTransform(XMVECTOR* pScale, XMVECTOR* pRotation, XMVECTOR* pPosition) const;

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
		void XM_CALLCONV Scale(FXMVECTOR scale) { XMStoreFloat3A(&m_scale, XMVectorMultiply(XMLoadFloat3A(&m_scale), scale)); }
		void Scale(FLOAT scale) { XMStoreFloat3A(&m_scale, XMVectorScale(XMLoadFloat3A(&m_scale), scale)); }
		void Scale(FLOAT sx, FLOAT sy, FLOAT sz) { m_scale.x *= sx; m_scale.y *= sy; m_scale.z *= sz; }
		void ScaleX(FLOAT sx) { m_scale.x *= sx; }
		void ScaleY(FLOAT sy) { m_scale.y *= sy; }
		void ScaleZ(FLOAT sz) { m_scale.z *= sz; }

		// 로컬 스케일을 재설정합니다.
		void XM_CALLCONV SetScale(FXMVECTOR scale) { XMStoreFloat3A(&m_scale, scale); }
		void SetScale(FLOAT sx, FLOAT sy, FLOAT sz) { m_scale.x = sx; m_scale.y = sy; m_scale.z = sz; }
		void SetScaleX(FLOAT sx) { m_scale.x = sx; }
		void SetScaleY(FLOAT sy) { m_scale.y = sy; }
		void SetScaleZ(FLOAT sz) { m_scale.z = sz; }

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
		void SetPosition(FLOAT x, FLOAT y, FLOAT z) { m_position.x = x;	m_position.y = y; m_position.z = z;	}
		void SetPositionX(FLOAT x) { m_position.x = x; }
		void SetPositionY(FLOAT y) { m_position.y = y; }
		void SetPositionZ(FLOAT z) { m_position.z = z; }


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
