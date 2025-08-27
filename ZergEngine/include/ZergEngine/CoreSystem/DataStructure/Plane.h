#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Plane
	{
	public:
		Plane()
		{
			XMStoreFloat4A(&m_equation, XMVectorZero());
		}
		Plane(FXMVECTOR equation)
		{
			XMStoreFloat4A(&m_equation, equation);
		}
		void Normalize() { XMStoreFloat4A(&m_equation, XMPlaneNormalize(XMLoadFloat4A(&m_equation))); }
		void NormalizeEst() { XMStoreFloat4A(&m_equation, XMPlaneNormalizeEst(XMLoadFloat4A(&m_equation))); }
	public:
		XMFLOAT4A m_equation;
	};
}
