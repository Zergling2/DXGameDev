#pragma once

#include <ZergEngine\Common\EngineHelper.h>

namespace ze
{
	class RasterizerState
	{
	public:
		RasterizerState()
			: m_pRasterizerState(nullptr)
		{
		}
		~RasterizerState();
		void Init(ID3D11Device* pDevice, const D3D11_RASTERIZER_DESC* pDesc);
		void Release();

		inline ID3D11RasterizerState* GetComInterface() const { return m_pRasterizerState; }
	private:
		ID3D11RasterizerState* m_pRasterizerState;
	};

	class SamplerState
	{
	public:
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		// 연속 레지스터 바인딩으로 한번에 세팅
		class MeshTextureSampler
		{
			DECLARE_SLOT_NUMBER(0);
		};
		class SkyboxSampler
		{
			DECLARE_SLOT_NUMBER(1);
		};
		class TerrainTextureSampler
		{
			DECLARE_SLOT_NUMBER(2);
		};
		class HeightmapSampler
		{
			DECLARE_SLOT_NUMBER(3);
		};
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	public:
		SamplerState()
			: m_pSamplerState(nullptr)
		{
		}
		~SamplerState();

		void Init(ID3D11Device* pDevice, const D3D11_SAMPLER_DESC* pDesc);
		void Release();

		inline ID3D11SamplerState* GetComInterface() const { return m_pSamplerState; }
	private:
		ID3D11SamplerState* m_pSamplerState;
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
		CAMERA_MERGE,

		// Number of depth stencil states
		COUNT
	};

	class DepthStencilState
	{
	public:
		DepthStencilState()
			: m_pDepthStencilState(nullptr)
		{
		}
		~DepthStencilState();

		void Init(ID3D11Device* pDevice, const D3D11_DEPTH_STENCIL_DESC* pDesc);
		void Release();

		inline ID3D11DepthStencilState* GetComInterface() const { return m_pDepthStencilState; }
	private:
		ID3D11DepthStencilState* m_pDepthStencilState;
	};
	
	class BlendState
	{
	public:
		BlendState()
			: m_pBlendState(nullptr)
			, m_blendFactor{ 1.0f, 1.0f, 1.0f, 1.0f }
			, m_mask(0xFFFFFFFF)
		{
		}
		~BlendState();

		void Init(ID3D11Device* pDevice, const D3D11_BLEND_DESC* pDesc);
		void Release();

		void SetBlendFactor(const FLOAT blendFactor[4]);
		inline const FLOAT* GetBlendFactor() const { return m_blendFactor; }
		inline void SetMask(UINT mask) { m_mask = mask; }
		inline UINT GetMask() const { return m_mask; }
		inline ID3D11BlendState* GetComInterface() const { return m_pBlendState; }
	private:
		ID3D11BlendState* m_pBlendState;
		FLOAT m_blendFactor[4];
		UINT m_mask;
	};
}
