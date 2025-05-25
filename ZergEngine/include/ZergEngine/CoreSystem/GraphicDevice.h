#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\Shader.h>
#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\RenderState.h>
#include <ZergEngine\CoreSystem\ShaderResource\VertexBuffer.h>

namespace ze
{
	class GraphicDeviceImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		friend class WindowImpl;
		ZE_DECLARE_SINGLETON(GraphicDeviceImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		bool LoadShaderByteCode(PCWSTR path, byte** ppByteCode, size_t* pSize);

		void CreateShaderAndInputLayout();
		void ReleaseShaderAndInputLayout();

		void CreateRasterizerStates();
		void ReleaseRasterizerStates();

		void CreateSamplerStates();
		void ReleaseSamplerStates();

		void CreateDepthStencilStates();
		void ReleaseDepthStencilStates();

		void CreateBlendStates();
		void ReleaseBlendStates();

		void CreateCommonVertexBuffers();
		void ReleaseCommonVertexBuffers();

		void OnResize();
		HRESULT GetFullscreenState(BOOL* pFullscreen);

		void CreateSupportedResolutionInfo();
		void CreateSupportedMSAAQualityInfo();
		void InitializeSwapChainAndDepthStencilBufferDesc();
	public:
		// Return max quality level
		UINT GetMSAAMaximumQuality(MSAA_SAMPLE_COUNT sampleCount);
		const DXGI_SWAP_CHAIN_DESC& GetSwapChainDesc() { return m_descSwapChain; }
		const D3D11_VIEWPORT& GetFullSwapChainViewport() const { return m_fullSwapChainViewport; }
		ID3D11Device* GetDeviceComInterface() { return m_pDevice; }
		ID3D11DeviceContext* GetImmediateContextComInterface() { return m_pImmediateContext; }

		IDXGISwapChain* GetSwapChainComInterface() const { return m_pSwapChain; }
		ID3D11RenderTargetView* GetSwapChainRTVComInterface() const { return m_pSwapChainRTV; }
		ID3D11DepthStencilView* GetSwapChainDSVComInterface() const { return m_pSwapChainDSV; }

		// Shaders getter
		ID3D11VertexShader* GetVSComInterface(VERTEX_SHADER_TYPE type) { return m_vs[static_cast<size_t>(type)].GetComInterface(); }
		ID3D11HullShader* GetHSComInterface(HULL_SHADER_TYPE type) { return m_hs[static_cast<size_t>(type)].GetComInterface(); }
		ID3D11DomainShader* GetDSComInterface(DOMAIN_SHADER_TYPE type) { return m_ds[static_cast<size_t>(type)].GetComInterface(); }
		ID3D11PixelShader* GetPSComInterface(PIXEL_SHADER_TYPE type) { return m_ps[static_cast<size_t>(type)].GetComInterface(); }
		ID3D11InputLayout* GetILComInterface(VERTEX_FORMAT_TYPE type) { return m_il[static_cast<size_t>(type)].GetComInterface(); }
		ID3D11Buffer* GetVBComInterface(VERTEX_BUFFER_TYPE type) { return m_vb[static_cast<size_t>(type)].GetComInterface(); }
		// Render state getter
		ID3D11RasterizerState* GetRSComInterface(RASTERIZER_FILL_MODE fm, RASTERIZER_CULL_MODE cm) const
		{
			return m_rs[static_cast<size_t>(fm)][static_cast<size_t>(cm)].GetComInterface();
		}
		ID3D11SamplerState* GetSSComInterface(TEXTURE_FILTERING_OPTION tfo) const
		{
			return m_ss[static_cast<size_t>(tfo)].GetComInterface();
		}
		ID3D11SamplerState* GetSkyboxSamplerComInterface() const
		{
			return m_ssSkybox.GetComInterface();
		}
		ID3D11SamplerState* GetHeightmapSamplerComInterface() const
		{
			return m_ssHeightMap.GetComInterface();
		}
		ID3D11DepthStencilState* GetDSSComInterface(DEPTH_STENCIL_STATETYPE dst) const
		{
			return m_dss[static_cast<size_t>(dst)].GetComInterface();
		}
		ID3D11BlendState* GetBSComInterface(BLEND_STATETYPE bst) const
		{
			return m_bs[static_cast<size_t>(bst)].GetComInterface();
		}
	private:
		DXGI_ADAPTER_DESC m_descAdapter;
		DXGI_SWAP_CHAIN_DESC m_descSwapChain;
		D3D11_TEXTURE2D_DESC m_descDepthStencil;
		D3D11_VIEWPORT m_fullSwapChainViewport;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pImmediateContext;
		std::vector<DXGI_MODE_DESC> m_supportedResolution;
		std::vector<std::pair<MSAA_SAMPLE_COUNT, UINT>> m_supportedMSAA;
		IDXGISwapChain* m_pSwapChain;
		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
		ID3D11RenderTargetView* m_pSwapChainRTV;
		ID3D11DepthStencilView* m_pSwapChainDSV;

		VertexShader m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::COUNT)];
		HullShader m_hs[static_cast<size_t>(HULL_SHADER_TYPE::COUNT)];
		DomainShader m_ds[static_cast<size_t>(DOMAIN_SHADER_TYPE::COUNT)];
		PixelShader m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COUNT)];
		InputLayout m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::COUNT)];
		VertexBuffer m_vb[static_cast<size_t>(VERTEX_BUFFER_TYPE::COUNT)];

		RasterizerState m_rs[static_cast<size_t>(RASTERIZER_FILL_MODE::COUNT)][static_cast<size_t>(RASTERIZER_CULL_MODE::COUNT)];
		SamplerState m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::COUNT)];
		SamplerState m_ssSkybox;
		SamplerState m_ssHeightMap;
		DepthStencilState m_dss[static_cast<size_t>(DEPTH_STENCIL_STATETYPE::COUNT)];
		BlendState m_bs[static_cast<size_t>(BLEND_STATETYPE::COUNT)];
	};

	extern GraphicDeviceImpl GraphicDevice;
}
