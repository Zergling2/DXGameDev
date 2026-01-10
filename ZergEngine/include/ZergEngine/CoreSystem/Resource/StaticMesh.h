#pragma once

#include <ZergEngine\CoreSystem\DataStructure\MeshSubset.h>
#include <ZergEngine\CoreSystem\DataStructure\Aabb.h>
#include <string>
#include <vector>

namespace ze
{
	class StaticMesh
	{
		friend class ResourceLoader;
	public:
		StaticMesh(PCWSTR name)
			: m_subsets()
			, m_name(name)
			, m_aabb()
			, m_cpVB()
			, m_cpIB()
			// , m_vft(VertexFormatType::UNKNOWN)
		{
		}
		PCWSTR GetName() const { return m_name.c_str(); }
		const Aabb& GetAabb() const { return m_aabb; }
		ID3D11Buffer* GetVertexBuffer() const { return m_cpVB.Get(); }
		ID3D11Buffer* GetIndexBuffer() const { return m_cpIB.Get(); }
		// VertexFormatType GetVertexFormatType() const { return m_vft; }
	public:
		std::vector<MeshSubset> m_subsets;
	private:
		std::wstring m_name;
		Aabb m_aabb;
		ComPtr<ID3D11Buffer> m_cpVB;
		ComPtr<ID3D11Buffer> m_cpIB;
		// VertexFormatType m_vft;
	};
}
