#pragma once

#include <ZergEngine\CoreSystem\InputLayout.h>

namespace ze
{
	class Material;

	class Subset
	{
		friend class ResourceLoader;
	public:
		Subset()
			: m_material(nullptr)
			, m_indexCount(0)
			, m_startIndexLocation(0)
		{
		}
		~Subset() = default;
		inline uint32_t GetIndexCount() const { return m_indexCount; }
		inline uint32_t GetStartIndexLocation() const { return m_startIndexLocation; }
	public:
		std::shared_ptr<Material> m_material;
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
			, m_pVB(nullptr)
			, m_pIB(nullptr)
			, m_vft(VERTEX_FORMAT_TYPE::UNKNOWN)
		{
		}
		~Mesh();
		PCWSTR GetName() const { return m_name.c_str(); }
		ID3D11Buffer* GetVBComInterface() const { return m_pVB; }
		ID3D11Buffer* GetIBComInterface() const { return m_pIB; }
		VERTEX_FORMAT_TYPE GetVertexFormatType() const { return m_vft; }
	public:
		std::vector<Subset> m_subsets;
	private:
		std::wstring m_name;
		ID3D11Buffer* m_pVB;
		ID3D11Buffer* m_pIB;
		VERTEX_FORMAT_TYPE m_vft;
	};
}
