#pragma once

#include <functional>

namespace ze
{
	template<typename T>
	std::function<bool()> MakeUIHandler(ComponentHandle<T> ch, void (T::*mf)())
	{
		std::function<bool()> handler = [ch, mf]()
			{
				T* pComponent = ch.ToPtr();

				if (pComponent == nullptr)
					return false;

				(pComponent->*mf)();

				return true;
			};

		return handler;
	}
}
