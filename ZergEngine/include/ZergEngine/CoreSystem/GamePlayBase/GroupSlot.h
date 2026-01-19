#pragma once

#include <cstdint>
#include <limits>

namespace ze
{
	// Group Slot Index Type
	using gsi_type = uint32_t;

	struct GroupSlot
	{
	public:
		GroupSlot(gsi_type index = (std::numeric_limits<gsi_type>::max)())
			: m_index(index)
		{
		}
	public:
		gsi_type m_index;
	};
}
