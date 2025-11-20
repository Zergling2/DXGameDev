#pragma once

#include <cstdint>

namespace ze
{
	class MeshSubset
	{
		friend class ResourceLoader;
	public:
		MeshSubset()
			: m_indexCount(0)
			, m_startIndexLocation(0)
		{
		}
		MeshSubset(uint32_t indexCount, uint32_t startIndexLocation)
			: m_indexCount(indexCount)
			, m_startIndexLocation(startIndexLocation)
		{
		}
		~MeshSubset() = default;
		uint32_t GetIndexCount() const { return m_indexCount; }
		uint32_t GetStartIndexLocation() const { return m_startIndexLocation; }
	private:
		uint32_t m_indexCount;
		uint32_t m_startIndexLocation;
	};
}
