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

	// ���� ������ ������Ʈ ���� ������Ʈ�� ���� ���� Z��.
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
		// �Ÿ��� �ݺ���ϴ� �� ���� (0.0f, 1.0f, 0.0f)
		// �Ÿ��� ������ �ݺ���ϴ� �� ���� (0.0f, 0.0f, 1.0f)
		XMFLOAT3 m_att;		// LitColor = A + (KdD + KsS) / (m_att.x + m_att.y * d + m_att.z * d^2)
	};

	// ���� ������ ������Ʈ ���� ������Ʈ�� ���� ���� Z��.
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

		// ����Ʈ����Ʈ �� ũ�� ���� (8.0f -> �뷫 45��)
		FLOAT m_spotExp;		// Kspot = max(-L dot dir, 0) ^ m_spot

		// Ex)
		// �Ÿ��� �ݺ���ϴ� �� ���� (0.0f, 1.0f, 0.0f)
		// �Ÿ��� ������ �ݺ���ϴ� �� ���� (0.0f, 0.0f, 1.0f)
		XMFLOAT3 m_att;		// LitColor = Kspot(A + (KdD + KsS) / (m_att.x + m_att.y * d + m_att.z * d^2))
	};
}
