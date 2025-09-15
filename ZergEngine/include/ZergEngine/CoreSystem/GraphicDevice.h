#pragma once

#include <ZergEngine\CoreSystem\Shader.h>
#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\RenderState.h>
#include <ZergEngine\CoreSystem\ShaderResource\VertexBuffer.h>
#include <ZergEngine\CoreSystem\Resource\Font.h>
#include <memory>
#include <vector>
#include <unordered_map>

namespace ze
{
	class GraphicDevice
	{
		friend class Runtime;
	public:
		static GraphicDevice* GetInstance() { return s_pInstance; }
	private:
		GraphicDevice();
		~GraphicDevice();

		static void CreateInstance();
		static void DestroyInstance();

		void Init(HWND hWnd, uint32_t width, uint32_t height, bool fullscreen);
		void UnInit();

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

		void ResizeBuffer(uint32_t width, uint32_t height);
		HRESULT SetFullscreenState(BOOL fullscreen);
		HRESULT GetFullscreenState(BOOL* pFullscreen);

		void CreateSupportedResolutionInfo();
		void CreateSupportedMSAAQualityInfo();
	public:
		// Return max quality level
		DXGI_FORMAT GetBackBufferFormat() const { return m_backBufferFormat; }
		UINT GetMSAAMaximumQuality(MultisamplingAntiAliasingMode sampleCount);
		const DXGI_SWAP_CHAIN_DESC& GetSwapChainDesc() { return m_descSwapChain; }
		const D3D11_VIEWPORT& GetEntireSwapChainViewport() const { return m_entireSwapChainViewport; }
		void UpdateEntireSwapChainViewport(uint32_t width, uint32_t height);
		uint32_t GetSwapChainWidth() const { return m_descSwapChain.BufferDesc.Width; }
		uint32_t GetSwapChainHeight() const { return m_descSwapChain.BufferDesc.Height; }
		float GetSwapChainWidthFlt() const { return m_swapChainSizeFlt.x; }
		float GetSwapChainHeightFlt() const { return m_swapChainSizeFlt.y; }
		float GetSwapChainHalfWidthFlt() const { return m_swapChainHalfSizeFlt.x; }
		float GetSwapChainHalfHeightFlt() const { return m_swapChainHalfSizeFlt.y; }
		ID3D11Device* GetDeviceComInterface() { return m_cpDevice.Get(); }
		ID3D11DeviceContext* GetImmediateContextComInterface() { return m_cpImmediateContext.Get(); }

		IDXGISwapChain* GetSwapChainComInterface() const { return m_cpSwapChain.Get(); }
		ID3D11RenderTargetView* GetSwapChainRTVComInterface() const { return m_cpSwapChainRTV.Get(); }
		ID3D11DepthStencilView* GetSwapChainDSVComInterface() const { return m_cpSwapChainDSV.Get(); }
		ID3D11RenderTargetView* GetSwapChainRTV() const { return m_cpSwapChainRTV.Get(); }
		ID3D11DepthStencilView* GetSwapChainDSV() const { return m_cpSwapChainDSV.Get(); }
		ID2D1RenderTarget* GetD2DRenderTarget() const { return m_cpD2DRenderTarget.Get(); }
		ID2D1SolidColorBrush* GetD2DSolidColorBrush() const { return m_cpD2DSolidColorBrush.Get(); }
		std::shared_ptr<DWriteTextFormatWrapper> GetDWriteTextFormatWrapper(const TextFormat& tf);

		// Shaders getter
		ID3D11VertexShader* GetVSComInterface(VertexShaderType type) { return m_vs[static_cast<size_t>(type)].GetComInterface(); }
		ID3D11HullShader* GetHSComInterface(HullShaderType type) { return m_hs[static_cast<size_t>(type)].GetComInterface(); }
		ID3D11DomainShader* GetDSComInterface(DomainShaderType type) { return m_ds[static_cast<size_t>(type)].GetComInterface(); }
		ID3D11PixelShader* GetPSComInterface(PixelShaderType type) { return m_ps[static_cast<size_t>(type)].GetComInterface(); }
		ID3D11InputLayout* GetILComInterface(VertexFormatType type) { return m_il[static_cast<size_t>(type)].GetComInterface(); }
		ID3D11Buffer* GetVBComInterface(VertexBufferType type) { return m_vb[static_cast<size_t>(type)].GetComInterface(); }
		// Render state getter
		ID3D11RasterizerState* GetRSComInterface(RasterizerFillMode fillMode, RasterizerCullMode cullMode) const
		{
			return m_rs[static_cast<size_t>(fillMode)][static_cast<size_t>(cullMode)].GetComInterface();
		}
		ID3D11SamplerState* GetSSComInterface(TextureFilteringMode mode) const
		{
			return m_ss[static_cast<size_t>(mode)].GetComInterface();
		}
		ID3D11DepthStencilState* GetDSSComInterface(DepthStencilStateType type) const
		{
			return m_dss[static_cast<size_t>(type)].GetComInterface();
		}
		ID3D11BlendState* GetBSComInterface(BlendStateType type) const
		{
			return m_bs[static_cast<size_t>(type)].GetComInterface();
		}
	private:
		static GraphicDevice* s_pInstance;
		DXGI_FORMAT m_backBufferFormat;
		DXGI_ADAPTER_DESC m_descAdapter;
		DXGI_SWAP_CHAIN_DESC m_descSwapChain;
		XMFLOAT2 m_swapChainSizeFlt;
		XMFLOAT2 m_swapChainHalfSizeFlt;
		D3D11_TEXTURE2D_DESC m_descDepthStencil;
		D3D11_VIEWPORT m_entireSwapChainViewport;
		std::vector<DXGI_MODE_DESC> m_supportedResolution;
		std::vector<std::pair<MultisamplingAntiAliasingMode, UINT>> m_supportedMSAA;
		ComPtr<ID3D11Device> m_cpDevice;
		ComPtr<ID3D11DeviceContext> m_cpImmediateContext;
		ComPtr<ID2D1Factory> m_cpD2DFactory;
		ComPtr<IDWriteFactory> m_cpDWriteFactory;
		ComPtr<IDXGISwapChain> m_cpSwapChain;
		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
		ComPtr<ID3D11RenderTargetView> m_cpSwapChainRTV;
		ComPtr<ID3D11DepthStencilView> m_cpSwapChainDSV;
		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
		ComPtr<ID2D1RenderTarget> m_cpD2DRenderTarget;
		ComPtr<ID2D1SolidColorBrush> m_cpD2DSolidColorBrush;
		std::unordered_map<TextFormat, std::weak_ptr<DWriteTextFormatWrapper>, TextFormatHasher> m_fontMap;
		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收

		VertexShader m_vs[static_cast<size_t>(VertexShaderType::COUNT)];
		HullShader m_hs[static_cast<size_t>(HullShaderType::COUNT)];
		DomainShader m_ds[static_cast<size_t>(DomainShaderType::COUNT)];
		PixelShader m_ps[static_cast<size_t>(PixelShaderType::COUNT)];
		InputLayout m_il[static_cast<size_t>(VertexFormatType::COUNT)];
		VertexBuffer m_vb[static_cast<size_t>(VertexBufferType::COUNT)];

		RasterizerState m_rs[static_cast<size_t>(RasterizerFillMode::COUNT)][static_cast<size_t>(RasterizerCullMode::COUNT)];
		SamplerState m_ss[static_cast<size_t>(TextureFilteringMode::COUNT)];
		DepthStencilState m_dss[static_cast<size_t>(DepthStencilStateType::COUNT)];
		BlendState m_bs[static_cast<size_t>(BlendStateType::COUNT)];
	};
}
