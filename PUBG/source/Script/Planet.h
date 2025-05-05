#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class Planet : public ze::IScript
	{
	public:
		Planet()
			: m_orbitRadius(1.0f)
			, m_angularVelocity(90.0f)
			, m_angle(0.0f)
			, m_position(m_orbitRadius, 0.0f, 0.0f)
		{
			XMStoreFloat3A(&m_rotationAxis, XMVector3Normalize(XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f)));
		}
		virtual ~Planet() = default;

		virtual void Update() override;

		void SetOrbitRadius(float r) { m_orbitRadius = r; }
		float GetOrbitRadius() const { return m_orbitRadius; }
		void SetAngularVelocity(float v) { m_angularVelocity = v; }
		float GetAngularVelocity() const { return m_angularVelocity; }
	private:
		float m_orbitRadius;
		float m_angularVelocity;
		float m_angle;
		XMFLOAT3A m_rotationAxis;
		XMFLOAT3A m_position;	// Local position
	};
}
