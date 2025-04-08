#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\Common\EngineConstants.h>
#include <ZergEngine\CoreSystem\RenderState.h>
#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\Resource\Shader.h>

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

		void OnResize();
		HRESULT GetFullscreenState(BOOL* pFullscreen);

		void CreateShaderAndInputLayout();
		void ReleaseShaderAndInputLayout();

		void CreateRasterizerStates();
		void ReleaseRasterizerStates();
		
		void CreateSamplerStates();
		void ReleaseSamplerStates();
		
		void CreateDepthStencilStates();
		void ReleaseDepthStencilStates();

		std::vector<byte> LoadShaderByteCode(PCWSTR path);

		void CreateSupportedResolutionInfo();
		void CreateSupportedMSAAQualityInfo();
		void InitializeSwapChainAndDepthStencilBufferDesc();
	public:
		// Return max quality level
		UINT GetMSAAMaximumQuality(MSAA_SAMPLE_COUNT sampleCount);
		inline const DXGI_SWAP_CHAIN_DESC& GetSwapChainDesc() { return m_descSwapChain; }
		inline ID3D11Device* GetDeviceComInterface() { return m_pDevice; }
		inline ID3D11DeviceContext* GetImmContextComInterface() { return m_pImmContext; }

		// Shaders getter
		inline ID3D11VertexShader* GetVSComInterface(VERTEX_SHADER_TYPE type) { return m_vs[static_cast<size_t>(type)].GetComInterface(); }
		inline ID3D11HullShader* GetHSComInterface(HULL_SHADER_TYPE type) { return m_hs[static_cast<size_t>(type)].GetComInterface(); }
		inline ID3D11DomainShader* GetDSComInterface(DOMAIN_SHADER_TYPE type) { return m_ds[static_cast<size_t>(type)].GetComInterface(); }
		inline ID3D11PixelShader* GetPSComInterface(PIXEL_SHADER_TYPE type) { return m_ps[static_cast<size_t>(type)].GetComInterface(); }
		inline ID3D11InputLayout* GetILComInterface(VERTEX_FORMAT_TYPE type) { return m_il[static_cast<size_t>(type)].GetComInterface(); }

		// Render state getter
		inline ID3D11RasterizerState* GetRSComInterface(RASTERIZER_FILL_MODE fm, RASTERIZER_CULL_MODE cm) const { return m_rasterizerStates[static_cast<size_t>(fm)][static_cast<size_t>(cm)].GetComInterface(); }
		inline ID3D11SamplerState* GetSSComInterface(TEXTURE_FILTERING_OPTION tfo) const { return m_samplerStates[static_cast<size_t>(tfo)].GetComInterface(); }
		inline ID3D11DepthStencilState* GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE dst) const { return m_depthStencilStates[static_cast<size_t>(dst)].GetComInterface(); }
		inline ID3D11SamplerState* GetSkyboxSamplerComInterface() const { return m_skyboxSamplerState.GetComInterface(); }
		inline ID3D11SamplerState* GetHeightmapSamplerComInterface() const { return m_heightmapSamplerState.GetComInterface(); }

		inline IDXGISwapChain* GetSwapChainComInterface() const { return m_pSwapChain; }
		inline ID3D11RenderTargetView* GetSwapChainRTVComInterface() const { return m_pSwapChainRTV; }
		inline ID3D11DepthStencilView* GetSwapChainDSVComInterface() const { return m_pSwapChainDSV; }
	private:
		DXGI_ADAPTER_DESC m_descAdapter;
		DXGI_SWAP_CHAIN_DESC m_descSwapChain;
		D3D11_TEXTURE2D_DESC m_descDepthStencil;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pImmContext;
		std::vector<DXGI_MODE_DESC> m_supportedResolution;
		std::vector<std::pair<MSAA_SAMPLE_COUNT, UINT>> m_supportedMSAA;
		IDXGISwapChain* m_pSwapChain;
		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
		ID3D11RenderTargetView* m_pSwapChainRTV;
		ID3D11DepthStencilView* m_pSwapChainDSV;
		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
		VertexShader m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::COUNT)];
		HullShader m_hs[static_cast<size_t>(HULL_SHADER_TYPE::COUNT)];
		DomainShader m_ds[static_cast<size_t>(DOMAIN_SHADER_TYPE::COUNT)];
		PixelShader m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COUNT)];
		InputLayout m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::COUNT)];

		RasterizerState m_rasterizerStates[static_cast<size_t>(RASTERIZER_FILL_MODE::COUNT)][static_cast<size_t>(RASTERIZER_CULL_MODE::COUNT)];
		SamplerState m_samplerStates[static_cast<size_t>(TEXTURE_FILTERING_OPTION::COUNT)];
		DepthStencilState m_depthStencilStates[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::COUNT)];
		SamplerState m_skyboxSamplerState;
		SamplerState m_heightmapSamplerState;
	};

	extern GraphicDeviceImpl GraphicDevice;
}
