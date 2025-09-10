#pragma once

#include <ZergEngine\Common\EngineConstants.h>
#include <ZergEngine\CoreSystem\DataStructure\Aabb.h>

namespace ze
{
	class Material;

	class Subset
	{
		friend class ResourceLoader;
	public:
		Subset()
			: m_indexCount(0)
			, m_startIndexLocation(0)
		{
		}
		Subset(uint32_t indexCount, uint32_t startIndexLocation)
			: m_indexCount(indexCount)
			, m_startIndexLocation(startIndexLocation)
		{
		}
		~Subset() = default;
		uint32_t GetIndexCount() const { return m_indexCount; }
		uint32_t GetStartIndexLocation() const { return m_startIndexLocation; }
	private:
		uint32_t m_indexCount;
		uint32_t m_startIndexLocation;
	};

	class Mesh
	{
		friend class ResourceLoader;
	public:
		Mesh(PCWSTR name)
			: m_subsets()
			, m_name(name)
			, m_aabb()
			, m_cpVB()
			, m_cpIB()
			, m_vft(VERTEX_FORMAT_TYPE::UNKNOWN)
		{
		}
		PCWSTR GetName() const { return m_name.c_str(); }
		const Aabb& GetAabb() const { return m_aabb; }
		ID3D11Buffer* GetVBComInterface() const { return m_cpVB.Get(); }
		ID3D11Buffer* GetIBComInterface() const { return m_cpIB.Get(); }
		VERTEX_FORMAT_TYPE GetVertexFormatType() const { return m_vft; }
	public:
		std::vector<Subset> m_subsets;
	private:
		std::wstring m_name;
		Aabb m_aabb;
		ComPtr<ID3D11Buffer> m_cpVB;
		ComPtr<ID3D11Buffer> m_cpIB;
		VERTEX_FORMAT_TYPE m_vft;
	};
}
