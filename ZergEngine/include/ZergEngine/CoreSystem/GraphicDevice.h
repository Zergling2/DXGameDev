#pragma once

#include <ZergEngine\CoreSystem\Shader.h>
#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\RenderState.h>
#include <ZergEngine\CoreSystem\ShaderResource\VertexBuffer.h>
#include <ZergEngine\CoreSystem\ShaderResource\IndexBuffer.h>
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

		bool Init(HWND hWnd, uint32_t width, uint32_t height, bool fullscreen);
		void Shutdown();

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

		bool CreateCommonGraphicResources();
		void ReleaseCommonGraphicResources();

		bool ResizeBuffer(uint32_t width, uint32_t height);
		HRESULT SetFullscreenState(BOOL fullscreen);
		HRESULT GetFullscreenState(BOOL* pFullscreen);

		void CreateSupportedResolutionInfo();
		void CreateSupportedMSAAQualityInfo();

		bool CreateGraphicDeviceResources();
		void ReleaseGraphicDeviceResources();
	public:
		LPCWSTR GetAdapterDescription() const { return m_descAdapter.Description; }
		size_t GetAdapterDedicatedVideoMemory() const { return m_descAdapter.DedicatedVideoMemory; }
		size_t GetAdapterDedicatedSystemMemory() const { return m_descAdapter.DedicatedSystemMemory; }
		size_t GetAdapterSharedSystemMemory() const { return m_descAdapter.SharedSystemMemory; }

		// Return max quality level
		UINT GetMSAAMaximumQuality(MSAAMode sampleCount);
		const DXGI_SWAP_CHAIN_DESC& GetSwapChainDesc() { return m_swapChainDesc; }
		const D3D11_VIEWPORT& GetEntireSwapChainViewport() const { return m_entireSwapChainViewport; }
		uint32_t GetSwapChainWidth() const { return m_swapChainDesc.BufferDesc.Width; }
		uint32_t GetSwapChainHeight() const { return m_swapChainDesc.BufferDesc.Height; }
		float GetSwapChainWidthFlt() const { return m_swapChainSizeFlt.x; }
		float GetSwapChainHeightFlt() const { return m_swapChainSizeFlt.y; }
		float GetSwapChainHalfWidthFlt() const { return m_swapChainHalfSizeFlt.x; }
		float GetSwapChainHalfHeightFlt() const { return m_swapChainHalfSizeFlt.y; }
		ID3D11Device* GetDevice() const { return m_cpDevice.Get(); }
		ID3D11DeviceContext* GetImmediateContext() const { return m_cpImmediateContext.Get(); }

		IDXGISwapChain* GetSwapChain() const { return m_cpSwapChain.Get(); }
		ID3D11RenderTargetView* GetSwapChainRTV() const { return m_cpRTVSwapChain.Get(); }
		ID3D11DepthStencilView* GetSwapChainDSV() const { return m_cpDSVSwapChain.Get(); }
		ID2D1RenderTarget* GetSwapChainD2DRT() const { return m_cpD2DRTBackBuffer.Get(); }
		ID2D1SolidColorBrush* GetD2DSolidColorBrush() const { return m_cpD2DSolidColorBrush.Get(); }
		std::shared_ptr<DWriteTextFormatWrapper> GetDWriteTextFormatWrapper(const TextFormat& tf);

		// Shaders getter
		ID3D11VertexShader* GetVSComInterface(VertexShaderType type) { return m_vs[static_cast<size_t>(type)].Get(); }
		ID3D11HullShader* GetHSComInterface(HullShaderType type) { return m_hs[static_cast<size_t>(type)].Get(); }
		ID3D11DomainShader* GetDSComInterface(DomainShaderType type) { return m_ds[static_cast<size_t>(type)].Get(); }
		ID3D11PixelShader* GetPSComInterface(PixelShaderType type) { return m_ps[static_cast<size_t>(type)].Get(); }
		ID3D11InputLayout* GetILComInterface(VertexFormatType type) { return m_il[static_cast<size_t>(type)].Get(); }
		// Render state getter
		ID3D11RasterizerState* GetRSComInterface(RasterizerMode mode) const
		{
			return m_rs[static_cast<size_t>(mode)].Get();
		}
		ID3D11SamplerState* GetSSComInterface(TextureFilteringMode mode) const
		{
			return m_ss[static_cast<size_t>(mode)].Get();
		}
		ID3D11DepthStencilState* GetDSSComInterface(DepthStencilStateType type) const
		{
			return m_dss[static_cast<size_t>(type)].Get();
		}
		ID3D11BlendState* GetBSComInterface(BlendStateType type) const
		{
			return m_bs[static_cast<size_t>(type)].Get();
		}
		// ID3D11Buffer* GetVBShaded2DQuad() const { return m_vbShaded2DQuad.Get(); }
		ID3D11Buffer* GetVBCheckbox() const { return m_vbCheckbox.Get(); }
		ID3D11Buffer* GetVBPNTTQuad() const { return m_vbPNTTQuad.Get(); }
		ID3D11Buffer* GetPLVMeshVB() const { return m_plvMeshVB.Get(); }
		ID3D11Buffer* GetPLVMeshIB() const { return m_plvMeshIB.Get(); }
		ID3D11Buffer* GetSLVMeshVB() const { return m_slvMeshVB.Get(); }
		ID3D11Buffer* GetSLVMeshIB() const { return m_slvMeshIB.Get(); }
	private:
		static GraphicDevice* s_pInstance;
		DXGI_ADAPTER_DESC m_descAdapter;
		DXGI_SWAP_CHAIN_DESC m_swapChainDesc;
		XMFLOAT2 m_swapChainSizeFlt;
		XMFLOAT2 m_swapChainHalfSizeFlt;
		D3D11_VIEWPORT m_entireSwapChainViewport;
		std::vector<DXGI_MODE_DESC> m_supportedResolution;
		std::vector<std::pair<MSAAMode, UINT>> m_supportedMSAA;
		ComPtr<ID3D11Device> m_cpDevice;
		ComPtr<ID3D11DeviceContext> m_cpImmediateContext;
		ComPtr<ID2D1Factory> m_cpD2DFactory;
		ComPtr<IDWriteFactory> m_cpDWriteFactory;
		ComPtr<IDXGISwapChain> m_cpSwapChain;
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		ComPtr<ID3D11RenderTargetView> m_cpRTVSwapChain;
		ComPtr<ID3D11DepthStencilView> m_cpDSVSwapChain;
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		ComPtr<ID2D1RenderTarget> m_cpD2DRTBackBuffer;
		ComPtr<ID2D1SolidColorBrush> m_cpD2DSolidColorBrush;
		std::unordered_map<const TextFormat, std::weak_ptr<DWriteTextFormatWrapper>, TextFormatHasher> m_fontMap;
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

		VertexShader m_vs[static_cast<size_t>(VertexShaderType::COUNT)];
		HullShader m_hs[static_cast<size_t>(HullShaderType::COUNT)];
		DomainShader m_ds[static_cast<size_t>(DomainShaderType::COUNT)];
		PixelShader m_ps[static_cast<size_t>(PixelShaderType::COUNT)];
		InputLayout m_il[static_cast<size_t>(VertexFormatType::COUNT)];

		RasterizerState m_rs[static_cast<size_t>(RasterizerMode::COUNT)];
		SamplerState m_ss[static_cast<size_t>(TextureFilteringMode::COUNT)];
		DepthStencilState m_dss[static_cast<size_t>(DepthStencilStateType::COUNT)];
		BlendState m_bs[static_cast<size_t>(BlendStateType::COUNT)];

		// VertexBuffer m_vbShaded2DQuad;
		VertexBuffer m_vbCheckbox;
		VertexBuffer m_vbPNTTQuad;	// <TRIANGLESTRIP!!!>, 노말 매핑, 텍스쳐 타일 지원하는 정규화 쿼드
		VertexBuffer m_plvMeshVB;
		IndexBuffer m_plvMeshIB;
		VertexBuffer m_slvMeshVB;
		IndexBuffer m_slvMeshIB;
	};
}
