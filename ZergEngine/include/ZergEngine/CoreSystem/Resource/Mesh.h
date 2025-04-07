#pragma once

#include <ZergEngine\CoreSystem\InputLayout.h>

namespace ze
{
	class Material;

	class Subset
	{
		friend class ResourceManagerImpl;
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
		friend class ResourceManagerImpl;
	public:
		Mesh(PCWSTR name)
			: m_subsets()
			, m_name(name)
			, m_cpVB(nullptr)
			, m_cpIB(nullptr)
			, m_vft(VERTEX_FORMAT_TYPE::UNKNOWN)
		{
		}
		~Mesh() = default;
		inline PCWSTR GetName() const { return m_name.c_str(); }
		inline ID3D11Buffer* GetVBComInterface() const { return m_cpVB.Get(); }
		inline ID3D11Buffer* GetIBComInterface() const { return m_cpIB.Get(); }
		inline VERTEX_FORMAT_TYPE GetVertexFormatType() const { return m_vft; }
	public:
		std::vector<Subset> m_subsets;
	private:
		std::wstring m_name;
		ComPtr<ID3D11Buffer> m_cpVB;
		ComPtr<ID3D11Buffer> m_cpIB;
		VERTEX_FORMAT_TYPE m_vft;
	};
}
