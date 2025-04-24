#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	struct UIHitbox : public RECT
	{
	public:
		UIHitbox()
			: RECT{ 0, 0, 0, 0 }
		{
		}
		~UIHitbox() = default;
	};

	inline bool PointInUIHitbox(const UIHitbox* pUIHitbox, POINT pt) { return PtInRect(pUIHitbox, pt); }
}
