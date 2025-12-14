#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class ILight : public IComponent
	{
	public:
		static constexpr bool IsCreatable() { return true; }

		ILight(uint64_t id);
		virtual ~ILight() = default;
	public:
		XMFLOAT4A m_diffuse;
		XMFLOAT4A m_specular;
	};

	// 조명 방향은 컴포넌트 소유 오브젝트의 월드 방향 Z축.
	class DirectionalLight : public ILight
	{
	public:
		static constexpr ComponentType TYPE = ComponentType::DirectionalLight;
		static constexpr bool IsCreatable() { return ILight::IsCreatable(); }

		DirectionalLight();
		virtual ~DirectionalLight() = default;
		virtual ComponentType GetType() const override { return ComponentType::DirectionalLight; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	};

	class PointLight : public ILight
	{
	public:
		static constexpr ComponentType TYPE = ComponentType::PointLight;
		static constexpr bool IsCreatable() { return ILight::IsCreatable(); }

		PointLight();
		virtual ~PointLight() = default;
		virtual ComponentType GetType() const override { return ComponentType::PointLight; }

		XMFLOAT3 GetDistAtt() const { return m_distAtt; }
		void SetDistAttConstant(FLOAT constant) { m_distAtt.x = constant; }
		void SetDistAttLinear(FLOAT linear) { m_distAtt.y = linear; }
		void SetDistAttSquare(FLOAT square) { m_distAtt.z = square; }
		void SetDistAtt(FLOAT constant, FLOAT linear, FLOAT square) { m_distAtt.x = constant; m_distAtt.y = linear; m_distAtt.z = square; }
		void SetDistAtt(const XMFLOAT3& distAtt) { m_distAtt = distAtt; }
		void SetDistAtt(const XMFLOAT3A& distAtt) { m_distAtt = distAtt; }
		FLOAT GetRange() const { return m_range; }
		void SetRange(FLOAT range);
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		// 거리 기반 감쇠 매개변수
		// Ex)
		// 거리에 반비례 (0.0f, 1.0f, 0.0f)
		// 거리의 제곱에 반비례 (0.0f, 0.0f, 1.0f)
		XMFLOAT3 m_distAtt;
		FLOAT m_range;
	};

	// 조명 방향은 컴포넌트 소유 오브젝트의 월드 방향 Z축.
	class SpotLight : public ILight
	{
	public:
		static constexpr ComponentType TYPE = ComponentType::SpotLight;
		static constexpr bool IsCreatable() { return ILight::IsCreatable(); }

		SpotLight();
		virtual ~SpotLight() = default;
		virtual ComponentType GetType() const override { return ComponentType::SpotLight; }

		XMFLOAT3 GetDistAtt() const { return m_distAtt; }
		void SetDistAttConstant(FLOAT constant) { m_distAtt.x = constant; }
		void SetDistAttLinear(FLOAT linear) { m_distAtt.y = linear; }
		void SetDistAttSquare(FLOAT square) { m_distAtt.z = square; }
		void SetDistAtt(FLOAT constant, FLOAT linear, FLOAT square) { m_distAtt.x = constant; m_distAtt.y = linear; m_distAtt.z = square; }
		void SetDistAtt(const XMFLOAT3& distAtt) { m_distAtt = distAtt; }
		void SetDistAtt(const XMFLOAT3A& distAtt) { m_distAtt = distAtt; }
		FLOAT GetInnerConeAngle() const { return m_innerConeAngle; }
		void SetInnerConeAngle(FLOAT angle) { m_innerConeAngle = angle; }
		FLOAT GetOuterConeAngle() const { return m_outerConeAngle; }
		void SetOuterConeAngle(FLOAT angle) { m_outerConeAngle = angle; }
		FLOAT GetRange() const { return m_range; }
		void SetRange(FLOAT range);
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		// 거리 기반 감쇠 매개변수
		// Ex)
		// 거리에 반비례 (0.0f, 1.0f, 0.0f)
		// 거리의 제곱에 반비례 (0.0f, 0.0f, 1.0f)
		XMFLOAT3 m_distAtt;

		FLOAT m_innerConeAngle;		// radian
		FLOAT m_outerConeAngle;		// radian

		FLOAT m_range;
	};
}
