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

		ID3D11RasterizerState* GetComInterface() const { return m_pRasterizerState; }
	private:
		ID3D11RasterizerState* m_pRasterizerState;
	};

	class SamplerState
	{
	public:
		SamplerState()
			: m_pSamplerState(nullptr)
		{
		}
		~SamplerState();

		void Init(ID3D11Device* pDevice, const D3D11_SAMPLER_DESC* pDesc);
		void Release();

		ID3D11SamplerState* GetComInterface() const { return m_pSamplerState; }
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

		ID3D11DepthStencilState* GetComInterface() const { return m_pDepthStencilState; }
	private:
		ID3D11DepthStencilState* m_pDepthStencilState;
	};
	
	enum class BLEND_STATE_TYPE
	{
		OPAQUE_,
		ALPHABLEND,
		ADDITIVE,
		NON_PREMULTIPLIED,

		COUNT
	};

	class BlendState
	{
	public:
		BlendState()
			: m_pBlendState(nullptr)
		{
		}
		~BlendState();

		void Init(ID3D11Device* pDevice, const D3D11_BLEND_DESC* pDesc);
		void Release();

		ID3D11BlendState* GetComInterface() const { return m_pBlendState; }
	private:
		ID3D11BlendState* m_pBlendState;
	};
}
