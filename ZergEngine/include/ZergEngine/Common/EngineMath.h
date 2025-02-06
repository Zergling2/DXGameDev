#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
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
	};
}
