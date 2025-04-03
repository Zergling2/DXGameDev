#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class ILight : public IComponent
	{
	public:
		ILight()
			: m_ambient(1.0f, 1.0f, 1.0f, 1.0f)
			, m_diffuse(1.0f, 1.0f, 1.0f, 1.0f)
			, m_specular(1.0f, 1.0f, 1.0f, 1.0f)
		{
		}
		virtual ~ILight() = default;
	public:
		XMFLOAT4A m_ambient;
		XMFLOAT4A m_diffuse;
		XMFLOAT4A m_specular;
	};

	// 조명 방향은 컴포넌트 소유 오브젝트의 월드 방향 Z축.
	class DirectionalLight : public ILight
	{
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::DIRECTIONAL_LIGHT;
		DirectionalLight();
		virtual ~DirectionalLight() = default;
		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::DIRECTIONAL_LIGHT; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	};

	class PointLight : public ILight
	{
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::POINT_LIGHT;
		PointLight();
		virtual ~PointLight() = default;
		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::POINT_LIGHT; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	public:
		FLOAT m_range;

		// Ex)
		// 거리에 반비례하는 점 광원 (0.0f, 1.0f, 0.0f)
		// 거리의 제곱에 반비례하는 점 광원 (0.0f, 0.0f, 1.0f)
		XMFLOAT3 m_att;		// LitColor = A + (KdD + KsS) / (m_att.x + m_att.y * d + m_att.z * d^2)
	};

	// 조명 방향은 컴포넌트 소유 오브젝트의 월드 방향 Z축.
	class SpotLight : public ILight
	{
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::SPOT_LIGHT;
		SpotLight();
		virtual ~SpotLight() = default;
		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::SPOT_LIGHT; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	public:
		FLOAT m_range;

		// 스포트라이트 콘 크기 조절 (8.0f -> 대략 45도)
		FLOAT m_spotExp;		// Kspot = max(-L dot dir, 0) ^ m_spot

		// Ex)
		// 거리에 반비례하는 점 광원 (0.0f, 1.0f, 0.0f)
		// 거리의 제곱에 반비례하는 점 광원 (0.0f, 0.0f, 1.0f)
		XMFLOAT3 m_att;		// LitColor = Kspot(A + (KdD + KsS) / (m_att.x + m_att.y * d + m_att.z * d^2))
	};
}
