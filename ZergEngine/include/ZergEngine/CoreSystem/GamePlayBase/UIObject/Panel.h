#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>

namespace ze
{
	class Panel : public ISizeColorUIObject
	{
		friend class RendererImpl;
		friend class UIObjectManagerImpl;
	public:
		Panel(UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Panel() = default;
	};
}
