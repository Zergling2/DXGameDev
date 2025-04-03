#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class IResouce abstract
	{
	public:
		virtual ~IResouce()
		{
		}
		inline uint64_t GetResourceId() const { return m_resourceId; }
	private:
		uint64_t m_resourceId;
	};
}
