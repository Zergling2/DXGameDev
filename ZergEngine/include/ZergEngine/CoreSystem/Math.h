#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Math
	{
	public:
		template<typename T>
		static inline void Clamp(T& val, T min, T max)
		{
			if (val < min)
				val = min;
			else if (val > max)
				val = max;
		}

		template <typename T>
		static constexpr bool IsPowerOf2(T n)
		{
			return n > 0 && (n & (n - 1)) == 0;
		}

		template <typename T>
		static constexpr T PowerOf2(T n)
		{
			return static_cast<T>(1) << n;
		}

		// ============================================================================
		// Graphics math
		
		// ���� ���������� ����ü ��� 6�� ��� �� ��ȯ
		static void ExtractFrustumPlanesInWorldSpace(const XMFLOAT4X4A* pViewProjMatrix, XMFLOAT4A planes[6]);
	};
}
