#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class Material;

	class Subset
	{
		friend class Graphics;
		friend class Resource;
	public:
		Subset();
		inline uint32_t GetIndexCount() const { return m_indexCount; }
		inline uint32_t GetStartIndexLocation() const { return m_startIndexLocation; }
	public:
		bool m_shadeSmooth;
		std::shared_ptr<Material> m_material;
	private:
		uint32_t m_indexCount;
		uint32_t m_startIndexLocation;
	};

	class Mesh
	{
		friend class Resource;
	public:
		Mesh(const wchar_t* name)
			: m_subsets()
			, m_name(name)
			, m_vb(nullptr)
			, m_ib(nullptr)
			, m_vft(VERTEX_FORMAT_TYPE::UNKNOWN)
			// , m_if(DXGI_FORMAT::DXGI_FORMAT_R32_UINT)
		{
		}
		inline const wchar_t* GetName() const { return m_name.c_str(); }
		inline ID3D11Buffer* GetVertexBufferInterface() const { return m_vb.Get(); }
		inline ID3D11Buffer* GetIndexBufferInterface() const { return m_ib.Get(); }
		inline VERTEX_FORMAT_TYPE GetVertexFormatType() const { return m_vft; }
	public:
		std::vector<Subset> m_subsets;
	private:
		std::wstring m_name;
		ComPtr<ID3D11Buffer> m_vb;
		ComPtr<ID3D11Buffer> m_ib;
		VERTEX_FORMAT_TYPE m_vft;
		// DXGI_FORMAT m_if;       // index format
	};
}