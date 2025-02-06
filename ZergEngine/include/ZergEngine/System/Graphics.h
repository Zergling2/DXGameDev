#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class Graphics
	{
		friend class Engine;
		friend class Screen;
		friend class Resource;
		enum STATE_FLAG : uint32_t
		{
			FINE = 0x00000000,
			RENDERING_DISABLED = 0x00000001,
			RESIZE_REQUIRED = 0x00000002,
			STATUS_OCCLUDED = 0x00000004
		};
	private:
		static sysresult SysStartup(const EngineSystemInitDescriptor& desc);
		static void SysCleanup();
	public:
		static inline const D3D11_TEXTURE2D_DESC& GetDepthStencilDescriptor() { return Graphics::s_descDepthStencil; }
		static inline const DXGI_SWAP_CHAIN_DESC& GetSwapChainDescriptor() { return Graphics::s_descSwapChain; }
		static inline const DXGI_ADAPTER_DESC& GetAdapterDescriptor() { return Graphics::s_descAdapter; }
		static void EnableRendering();
		static void DisableRendering();
		static bool IsRenderingEnabled();

		// Return max quality level
		static UINT CheckMSAAQuality(MSAACount sampleCount);
		static inline ID3D11Device* GetDevice() { return Graphics::s_device.Get(); }
		static inline ID3D11DeviceContext* GetImmediateContext() { return Graphics::s_immediateContext.Get(); }
	private:
		static bool QuerySupportedMSAAQuality();
		static void InitializeSwapChainAndDepthStencilBufferDescriptor();
		static void RequestResolution(uint32_t width, uint32_t height, bool fullscreen);
		static inline uint32_t GetBackBufferWidth() { return Graphics::s_backBufferResolution.width; }
		static inline uint32_t GetBackBufferHeight() { return Graphics::s_backBufferResolution.height; }
		static inline bool IsWindowFullscreen() { return Graphics::s_backBufferResolution.fullscreen; }
		static void CommitResolution();
		static void HandleDevice();
		static void RenderFrame();
		static std::vector<byte> LoadShaderByteCode(const wchar_t* path);

		static void ExtractFrustumPlanesInWorldSpace(XMMATRIX vp, XMFLOAT4A planes[6]);	// 월드 공간에서의 절두체 평면 6개 계산 및 반환
	private:
		static uint32_t s_flag;
		static std::vector<std::pair<MSAACount, UINT>> s_msaa;
		static DXGI_ADAPTER_DESC s_descAdapter;
		static DXGI_SWAP_CHAIN_DESC s_descSwapChain;
		static D3D11_TEXTURE2D_DESC s_descDepthStencil;
		static ComPtr<ID3D11Device> s_device;
		static ComPtr<ID3D11DeviceContext> s_immediateContext;
		// static DeviceContext s_deferredContext[];
		static ComPtr<IDXGISwapChain> s_swapChain;
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		static ComPtr<ID3D11RenderTargetView> s_rtvColorBuffer;
		static ComPtr<ID3D11DepthStencilView> s_dsvDepthStencilBuffer;
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		static Resolution s_backBufferResolution;
		static Resolution s_resolutionRequested;
		static D3D11_VIEWPORT s_fullScreenViewport;
	};

	class DepthStencilState
	{
		friend class Graphics;
	public:
		enum class DEPTH_STENCIL_STATE_TYPE
		{
			NORMAL = 0,
			SKYBOX,
			MIRROR,
			CAMERA_MERGE,
			COUNT
		};
	public:
		static inline DepthStencilState& GetState(DEPTH_STENCIL_STATE_TYPE dsst)
		{
			return DepthStencilState::s_items[static_cast<size_t>(dsst)];
		}
		inline ID3D11DepthStencilState* GetInterface() const { return m_interface.Get(); }
		inline ComPtr<ID3D11DepthStencilState> GetInterfaceComPtr() const { return m_interface; }
		inline UINT GetStencilRef() const { return m_stencilRef; }
		inline void SetStencilRef(UINT stencilRef) { m_stencilRef = stencilRef; }
	private:
		static int Init();
		static void Release();
	private:
		ComPtr<ID3D11DepthStencilState> m_interface;
		UINT m_stencilRef;
		static DepthStencilState s_items[static_cast<size_t>(DepthStencilState::DEPTH_STENCIL_STATE_TYPE::COUNT)];
	};

	class SkyboxResource
	{
		friend class Graphics;
	public:
		static inline ID3D11Buffer* GetVertexBufferInterface() { return SkyboxResource::s_vb.Get(); }
		static inline ID3D11Buffer* GetIndexBufferInterface() { return SkyboxResource::s_ib.Get(); }
	private:
		static int Init();
		static void Release();
	private:
		static ComPtr<ID3D11Buffer> s_vb;
		static ComPtr<ID3D11Buffer> s_ib;
	};

	class Screen
	{
		friend class Graphics;
	private:
		Screen();
	public:
		static uint32_t GetWidth();
		static uint32_t GetHeight();
		static bool IsFullscreen();
		static void SetResolution(uint32_t width, uint32_t height, bool fullscreen);
	};
}
