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

	bool PointInUIHitbox(const UIHitbox& uiHitbox, POINT pt) { return PtInRect(&uiHitbox, pt); }
}
