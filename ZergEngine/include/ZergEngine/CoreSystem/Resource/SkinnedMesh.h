#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\DataStructure\MeshSubset.h>
#include <string>
#include <vector>

namespace ze
{
	class SkinnedMesh
	{
		friend class ResourceLoader;
	public:
		SkinnedMesh(PCWSTR name)
			: m_subsets()
			, m_name(name)
			, m_cpVB()
			, m_cpIB()
		{
		}
		PCWSTR GetName() const { return m_name.c_str(); }
		ID3D11Buffer* GetVertexBuffer() const { return m_cpVB.Get(); }
		ID3D11Buffer* GetIndexBuffer() const { return m_cpIB.Get(); }
	public:
		std::vector<MeshSubset> m_subsets;
	private:
		std::wstring m_name;
		ComPtr<ID3D11Buffer> m_cpVB;
		ComPtr<ID3D11Buffer> m_cpIB;
	};
}
