#pragma once

#include <ZergEngine\Common\EngineHelper.h>

namespace ze
{
	class RasterizerState
	{
	public:
		RasterizerState()
			: m_cpInterface(nullptr)
		{
		}
		~RasterizerState() = default;
		void Init(ID3D11Device* pDevice, const D3D11_RASTERIZER_DESC* pDesc);
		inline void Release() { m_cpInterface.Reset(); }

		inline ID3D11RasterizerState* GetComInterface() const { return m_cpInterface.Get(); }
	private:
		ComPtr<ID3D11RasterizerState> m_cpInterface;
	};

	class SamplerState
	{
	public:
		class MeshTextureSampler
		{
			DECLARE_SLOT_NUMBER(0);
		};

		class SkyboxSampler
		{
			DECLARE_SLOT_NUMBER(0);
		};

		class TerrainTextureSampler
		{
			DECLARE_SLOT_NUMBER(0);
		};

		class HeightmapSampler
		{
			DECLARE_SLOT_NUMBER(1);
		};
	public:
		SamplerState()
			: m_cpInterface(nullptr)
		{
		}
		~SamplerState() = default;

		void Init(ID3D11Device* pDevice, const D3D11_SAMPLER_DESC* pDesc);
		inline void Release() { m_cpInterface.Reset(); }

		inline ID3D11SamplerState* GetComInterface() const { return m_cpInterface.Get(); }
	private:
		ComPtr<ID3D11SamplerState> m_cpInterface;
	};

	enum class DEPTH_STENCIL_STATE_TYPE
	{
		// DepthEnable = TRUE
		// DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL
		// DepthFunc = D3D11_COMPARISON_LESS
		// StencilEnable = FALSE
		STANDARD,

		// DepthEnable = TRUE
		// DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL
		// DepthFunc = D3D11_COMPARISON_LESS_EQUAL (스카이박스의 모든 프래그먼트를 NDC에서 z = 1.0f에 위치시키므로)
		// StencilEnable = FALSE
		SKYBOX,


		MIRROR,

		// DepthEnable = FALSE
		// StencilEnable = FALSE
		RENDER_RESULT_MERGE,

		// Number of depth stencil states
		COUNT
	};

	class DepthStencilState
	{
	public:
		DepthStencilState()
			: m_cpInterface(nullptr)
		{
		}
		~DepthStencilState() = default;

		void Init(ID3D11Device* pDevice, const D3D11_DEPTH_STENCIL_DESC* pDesc);
		inline void Release() { m_cpInterface.Reset(); }

		inline ID3D11DepthStencilState* GetComInterface() const { return m_cpInterface.Get(); }
	private:
		ComPtr<ID3D11DepthStencilState> m_cpInterface;
	};
	
	class BlendState
	{
	public:
		BlendState()
			: m_cpInterface(nullptr)
		{
		}
		~BlendState() = default;

		void Init(ID3D11Device* pDevice, const D3D11_BLEND_DESC* pDesc);
		inline void Release() { m_cpInterface.Reset(); }

		inline ID3D11BlendState* GetComInterface() const { return m_cpInterface.Get(); }
	private:
		ComPtr<ID3D11BlendState> m_cpInterface;
	};
}
