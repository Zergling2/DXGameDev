#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class RasterizerState
	{
	public:
		RasterizerState() = default;
		~RasterizerState() = default;
		void Init(ID3D11Device* pDevice, const D3D11_RASTERIZER_DESC* pDesc);
		void Release();

		ID3D11RasterizerState* GetComInterface() const { return m_cpState.Get(); }
	private:
		ComPtr<ID3D11RasterizerState> m_cpState;
	};

	class SamplerState
	{
	public:
		SamplerState() = default;
		~SamplerState() = default;

		void Init(ID3D11Device* pDevice, const D3D11_SAMPLER_DESC* pDesc);
		void Release();

		ID3D11SamplerState* GetComInterface() const { return m_cpState.Get(); }
	private:
		ComPtr<ID3D11SamplerState> m_cpState;
	};

	enum class DepthStencilStateType
	{
		Default,

		// DepthEnable = TRUE
		// DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL
		// DepthFunc = D3D11_COMPARISON_LESS_EQUAL (��ī�̹ڽ��� ��� �����׸�Ʈ�� NDC���� z = 1.0f�� ��ġ��Ű�Ƿ�)
		// StencilEnable = FALSE
		Skybox,

		Mirror,

		// Depth read only, No write
		DepthReadOnly,

		// DepthEnable = FALSE
		// StencilEnable = FALSE
		NoDepthStencilTest,
		// ��������������������������������������������
		COUNT
	};

	class DepthStencilState
	{
	public:
		DepthStencilState() = default;
		~DepthStencilState() = default;

		void Init(ID3D11Device* pDevice, const D3D11_DEPTH_STENCIL_DESC* pDesc);
		void Release();

		ID3D11DepthStencilState* GetComInterface() const { return m_cpState.Get(); }
	private:
		ComPtr<ID3D11DepthStencilState> m_cpState;
	};

	enum class BlendStateType
	{
		Opaque,


		AlphaBlend,

		// Color buffer���� ���� X, Depth/Stencil ���ۿ��� ���� �� ���
		NoColorWrite,
		// ��������������������������������������������
		COUNT
	};

	class BlendState
	{
	public:
		BlendState() = default;
		~BlendState() = default;

		void Init(ID3D11Device* pDevice, const D3D11_BLEND_DESC* pDesc);
		void Release();

		ID3D11BlendState* GetComInterface() const { return m_cpState.Get(); }
	private:
		ComPtr<ID3D11BlendState> m_cpState;
	};
}
