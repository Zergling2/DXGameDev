#include <ZergEngine\System\Graphics.h>
#include <ZergEngine\System\Runtime.h>
#include <ZergEngine\System\SceneManager.h>
#include <ZergEngine\System\Scene.h>
#include <ZergEngine\System\ComponentSystem.h>
#include <ZergEngine\System\GameObject.h>
#include <ZergEngine\System\Debug.h>
#include <ZergEngine\System\FileSystem.h>
#include <ZergEngine\System\Mesh.h>
#include <ZergEngine\System\Terrain.h>
#include <ZergEngine\System\Shader\Shader.h>
#include <ZergEngine\System\Shader\InputLayout.h>
#include <ZergEngine\System\Component\MeshRenderer.h>
#include <ZergEngine\System\Component\Camera.h>
#include <ZergEngine\System\Component\Light.h>

using namespace zergengine;

static LPCWSTR const THIS_FILE_NAME = L"Graphics.cpp";

class RasterizerState
{
	friend class Graphics;
public:
	enum class FILL_MODE
	{
		WIREFRAME,
		SOLID,
		COUNT
	};
	enum class CULL_MODE
	{
		NONE,
		FRONT,
		BACK,
		COUNT
	};
public:
	static inline RasterizerState& GetState(FILL_MODE fm, CULL_MODE cm)
	{
		return s_items[static_cast<size_t>(fm)][static_cast<size_t>(cm)];
	}
	inline ID3D11RasterizerState* GetInterface() const { return m_interface.Get(); }
	inline ComPtr<ID3D11RasterizerState> GetInterfaceComPtr() const { return m_interface; }
private:
	static int Init();
	static void Release();
private:
	ComPtr<ID3D11RasterizerState> m_interface;
	static RasterizerState s_items[static_cast<size_t>(FILL_MODE::COUNT)][static_cast<size_t>(CULL_MODE::COUNT)];
};

class SamplerState
{
	friend class Graphics;
public:
	enum class TEXTURE_FILTERING_OPTION
	{
		POINT,
		BILINEAR,
		TRILINEAR,
		ANISOTROPIC_2X,
		ANISOTROPIC_4X,
		ANISOTROPIC_8X,
		ANISOTROPIC_16X,
		COUNT
	};
	static inline SamplerState& GetState(TEXTURE_FILTERING_OPTION tfopt)
	{
		return SamplerState::s_items[static_cast<size_t>(tfopt)];
	}
	inline ID3D11SamplerState* GetInterface() const { return m_interface.Get(); }
	inline ComPtr<ID3D11SamplerState> GetInterfaceComPtr() const { return m_interface; }
private:
	static int Init();
	static void Release();
private:
	ComPtr<ID3D11SamplerState> m_interface;
	static SamplerState s_items[static_cast<size_t>(SamplerState::TEXTURE_FILTERING_OPTION::COUNT)];
};

/*
class BlendState
{
	friend class Graphics;
public:
private:
	ComPtr<ID3D11BlendState> m_interface;
	static BlendState s_items[1];
};
*/

class CameraMergerQuadResource
{
	friend class Graphics;
private:
	static int Init();
	static void Release();

	// [0] LEFT BOTTOM
	// [1] LEFT TOP
	// [2] RIGHT BOTTOM
	// [3] RIGHT UP
private:
	static ComPtr<ID3D11Buffer> s_vb;	// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP!!
};

RasterizerState RasterizerState::s_items[][static_cast<size_t>(RasterizerState::CULL_MODE::COUNT)];
DepthStencilState DepthStencilState::s_items[];
SamplerState SamplerState::s_items[];
// BlendState BlendState::s_items[];
ComPtr<ID3D11Buffer> SkyboxResource::s_vb;
ComPtr<ID3D11Buffer> SkyboxResource::s_ib;
ComPtr<ID3D11Buffer> CameraMergerQuadResource::s_vb;

uint32_t Graphics::s_flag = Graphics::STATE_FLAG::FINE;
std::vector<std::pair<MSAACount, UINT>> Graphics::s_msaa;
DXGI_ADAPTER_DESC Graphics::s_descAdapter;
DXGI_SWAP_CHAIN_DESC Graphics::s_descSwapChain;
D3D11_TEXTURE2D_DESC Graphics::s_descDepthStencil;
ComPtr<ID3D11Device> Graphics::s_device;
ComPtr<ID3D11DeviceContext> Graphics::s_immediateContext;
ComPtr<IDXGISwapChain> Graphics::s_swapChain;
ComPtr<ID3D11RenderTargetView> Graphics::s_rtvColorBuffer;
ComPtr<ID3D11DepthStencilView> Graphics::s_dsvDepthStencilBuffer;
Resolution Graphics::s_backBufferResolution(0, 0, false);
Resolution Graphics::s_resolutionRequested(0, 0, false);
D3D11_VIEWPORT Graphics::s_fullScreenViewport;

sysresult Graphics::SysStartup(const EngineSystemInitDescriptor& desc)
{
	HRESULT hr;

	Graphics::s_flag |= Graphics::STATE_FLAG::RESIZE_REQUIRED;
	Graphics::s_resolutionRequested.width = desc.width;
	Graphics::s_resolutionRequested.height = desc.height;
	Graphics::s_resolutionRequested.fullscreen = desc.fullscreen;
	Graphics::s_fullScreenViewport.TopLeftX = 0.0f;
	Graphics::s_fullScreenViewport.TopLeftY = 0.0f;
	Graphics::s_fullScreenViewport.MinDepth = 0.0f;
	Graphics::s_fullScreenViewport.MaxDepth = 1.0f;

#if defined(DEBUG) || defined(_DEBUG)
	const UINT createDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#else
	const UINT createDeviceFlag = 0;
#endif
	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	D3D_FEATURE_LEVEL maxSupportedFeatureLevel;

	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlag,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		Graphics::s_device.GetAddressOf(),
		&maxSupportedFeatureLevel,
		Graphics::s_immediateContext.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	if (maxSupportedFeatureLevel < D3D_FEATURE_LEVEL_11_0)
		return -1;

	// ж萄錚橫縑憮 雖錳ж朝 詩じ價Ы葭 蘋遴お 羹觼
	// FEATURE_LEVEL_11_0 devices are required to support 4x MSAA for all render target formats,
	// and 8x MSAA for all render target formats except R32G32B32A32 formats.
	if (!Graphics::QuerySupportedMSAAQuality())
		return -1;

	Graphics::InitializeSwapChainAndDepthStencilBufferDescriptor();

	// 蝶諜 羹檣 儅撩
	// First, Get IDXGIFactory instance
	// DirectX Graphics Infrastructure(DXGI)
	ComPtr<IDXGIDevice> dxgiDevice;
	ComPtr<IDXGIAdapter> dxgiAdapter;
	ComPtr<IDXGIFactory> dxgiFactory;

	// Use the IDXGIFactory instance that was used to create the device! (by COM queries)
	hr = Graphics::GetDevice()->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgiDevice.GetAddressOf()));
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}
	
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(dxgiAdapter.GetAddressOf()));
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	// 橫湘攪 薑爾 �僱�
	hr = dxgiAdapter->GetDesc(&Graphics::s_descAdapter);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}
	
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	// Create a swap chain.
	hr = dxgiFactory->CreateSwapChain(Graphics::s_device.Get(), &Graphics::s_descSwapChain, Graphics::s_swapChain.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	// Detect if newly created full-screen swap chain isn't actually full screen.
	if (Graphics::s_resolutionRequested.fullscreen)
	{
		IDXGIOutput* pTarget = nullptr;
		BOOL isFullscreen;

		if (SUCCEEDED(Graphics::s_swapChain->GetFullscreenState(&isFullscreen, &pTarget)))
		{
			if (pTarget)
				pTarget->Release();
		}
		else
		{
			isFullscreen = FALSE;
		}

		// If not full screen, enable full screen again.
		if (!isFullscreen)
		{
			ShowWindow(Engine::GetMainWindowHandle(), SW_MINIMIZE);
			ShowWindow(Engine::GetMainWindowHandle(), SW_RESTORE);
			Graphics::s_swapChain->SetFullscreenState(TRUE, nullptr);
		}
	}

	// Disable ALT+ENTER window mode change
	hr = dxgiFactory->MakeWindowAssociation(Engine::GetMainWindowHandle(), DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE RASTERIZER STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	if (RasterizerState::Init() < 0)
		return -1;

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE DEPTHSTENCIL STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	if (DepthStencilState::Init() < 0)
		return -1;

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE SAMPLER STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	if (SamplerState::Init() < 0)
		return -1;

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE SKYBOX VB / IB 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	if (SkyboxResource::Init() < 0)
		return -1;

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE CAMERA MERGER QUAD 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	if (CameraMergerQuadResource::Init() < 0)
		return -1;

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE CONSTANT BUFFERS 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	{
		D3D11_BUFFER_DESC descBuffer;
		ZeroMemory(&descBuffer, sizeof(descBuffer));

		// 鼻熱 幗ぷ 奢鱔 樓撩 撮た
		descBuffer.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		descBuffer.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		descBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		descBuffer.MiscFlags = 0;
		descBuffer.StructureByteStride = 0;

		// 1.
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerFrame);
		hr = Graphics::GetDevice()->CreateBuffer(
			&descBuffer,
			nullptr,
			ConstantBuffer::PerFrame::s_cb.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 2.
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerCamera);
		hr = Graphics::GetDevice()->CreateBuffer(
			&descBuffer,
			nullptr,
			ConstantBuffer::PerCamera::s_cb.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 3.
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerMesh);
		hr = Graphics::GetDevice()->CreateBuffer(
			&descBuffer,
			nullptr,
			ConstantBuffer::PerMesh::s_cb.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 4.
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerSubset);
		hr = Graphics::GetDevice()->CreateBuffer(
			&descBuffer,
			nullptr,
			ConstantBuffer::PerSubset::s_cb.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 5.
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerRenderingResultMerge);
		hr = Graphics::GetDevice()->CreateBuffer(
			&descBuffer,
			nullptr,
			ConstantBuffer::PerRenderingResultMerge::s_cb.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}
	}

	// 收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE SHADERS AND INPUT LAYOUTS 收收收收收收收收收收收收收收收收收收收收收收收收收
	{
		std::vector<byte> shaderByteCode;

		// 1. POSITION
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\VSSkyboxTransform.cso");
		hr = Graphics::GetDevice()->CreateVertexShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::VSSkyboxTransform::s_vs.GetAddressOf()
		);
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\VSStandardTransformP.cso");
		hr = Graphics::GetDevice()->CreateVertexShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::VSStandardTransformP::s_vs.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}
		// 曲收 Create compatible Input Layout
		hr = Graphics::GetDevice()->CreateInputLayout(
			InputLayout::Position::s_ied,
			InputLayout::Position::INPUT_ELEMENT_COUNT,
			shaderByteCode.data(),
			shaderByteCode.size(),
			InputLayout::s_items[VertexFormat::Position::INPUT_LAYOUT_INDEX].GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 2. POSITION, COLOR
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\VSStandardTransformPC.cso");
		hr = Graphics::GetDevice()->CreateVertexShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::VSStandardTransformPC::s_vs.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}
		// 曲收 Create compatible Input Layout
		hr = Graphics::GetDevice()->CreateInputLayout(
			InputLayout::PositionColor::s_ied,
			InputLayout::PositionColor::INPUT_ELEMENT_COUNT,
			shaderByteCode.data(),
			shaderByteCode.size(),
			InputLayout::s_items[VertexFormat::PositionColor::INPUT_LAYOUT_INDEX].GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 3. POSITION, NORMAL
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\VSStandardTransformPN.cso");
		hr = Graphics::GetDevice()->CreateVertexShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::VSStandardTransformPN::s_vs.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}
		// 曲收 Create compatible Input Layout
		hr = Graphics::GetDevice()->CreateInputLayout(
			InputLayout::PositionNormal::s_ied,
			InputLayout::PositionNormal::INPUT_ELEMENT_COUNT,
			shaderByteCode.data(),
			shaderByteCode.size(),
			InputLayout::s_items[VertexFormat::PositionNormal::INPUT_LAYOUT_INDEX].GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 4. POSITION, TEXCOORD
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\VSStandardTransformPT.cso");
		hr = Graphics::GetDevice()->CreateVertexShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::VSStandardTransformPT::s_vs.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\VSRenderingResultMerge.cso");
		hr = Graphics::GetDevice()->CreateVertexShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::VSRenderingResultMerge::s_vs.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}
		// 曲收 Create compatible Input Layout
		hr = Graphics::GetDevice()->CreateInputLayout(
			InputLayout::PositionTexCoord::s_ied,
			InputLayout::PositionTexCoord::INPUT_ELEMENT_COUNT,
			shaderByteCode.data(),
			shaderByteCode.size(),
			InputLayout::s_items[VertexFormat::PositionTexCoord::INPUT_LAYOUT_INDEX].GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 5. POSITION, NORMAL, TEXCOORD
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\VSStandardTransformPNT.cso");
		hr = Graphics::GetDevice()->CreateVertexShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::VSStandardTransformPNT::s_vs.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}
		// 曲收 Create compatible Input Layout
		hr = Graphics::GetDevice()->CreateInputLayout(
			InputLayout::PositionNormalTexCoord::s_ied,
			InputLayout::PositionNormalTexCoord::INPUT_ELEMENT_COUNT,
			shaderByteCode.data(),
			shaderByteCode.size(),
			InputLayout::s_items[VertexFormat::PositionNormalTexCoord::INPUT_LAYOUT_INDEX].GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 6. POSITION, TEXCOORD0, TEXCOORD1
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\VSTerrainTransform.cso");
		hr = Graphics::GetDevice()->CreateVertexShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::VSTerrainTransform::s_vs.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}
		// 曲收 Create compatible Input Layout
		hr = Graphics::GetDevice()->CreateInputLayout(
			InputLayout::Terrain::s_ied,
			InputLayout::Terrain::INPUT_ELEMENT_COUNT,
			shaderByteCode.data(),
			shaderByteCode.size(),
			InputLayout::s_items[VertexFormat::Terrain::INPUT_LAYOUT_INDEX].GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 1. HSTerrainRendering
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\HSTerrainRendering.cso");
		hr = Graphics::GetDevice()->CreateHullShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::HSTerrainRendering::s_hs.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 1. DSTerrainRendering
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\DSTerrainRendering.cso");
		hr = Graphics::GetDevice()->CreateDomainShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::DSTerrainRendering::s_ds.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 1. PSSkyboxColoring
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\PSSkyboxColoring.cso");
		hr = Graphics::GetDevice()->CreatePixelShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::PSSkyboxColoring::s_ps.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 2. PSStandardColoringP
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\PSStandardColoringP.cso");
		hr = Graphics::GetDevice()->CreatePixelShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::PSStandardColoringP::s_ps.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 3. PSStandardColoringPC
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\PSStandardColoringPC.cso");
		hr = Graphics::GetDevice()->CreatePixelShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::PSStandardColoringPC::s_ps.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 4. PSStandardColoringPN
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\PSStandardColoringPN.cso");
		hr = Graphics::GetDevice()->CreatePixelShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::PSStandardColoringPN::s_ps.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 5. PSStandardColoringPT
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\PSStandardColoringPT.cso");
		hr = Graphics::GetDevice()->CreatePixelShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::PSStandardColoringPT::s_ps.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 6. PSMSRenderResultMerge
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\PSMSRenderingResultMerge.cso");
		hr = Graphics::GetDevice()->CreatePixelShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::PSMSRenderingResultMerge::s_ps.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 7. PSStandardColoringPNT
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\PSStandardColoringPNT.cso");
		hr = Graphics::GetDevice()->CreatePixelShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::PSStandardColoringPNT::s_ps.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}

		// 8. PSTerrainColoring
		shaderByteCode = Graphics::LoadShaderByteCode(L"Shader\\PSTerrainColoring.cso");
		hr = Graphics::GetDevice()->CreatePixelShader(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			ShaderComponent::PSTerrainColoring::s_ps.GetAddressOf()
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			return -1;
		}
	}

	Graphics::HandleDevice();

	return 0;
}

void Graphics::SysCleanup()
{
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE CONSTANT BUFFERS 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	ConstantBuffer::PerRenderingResultMerge::s_cb.Reset();
	ConstantBuffer::PerSubset::s_cb.Reset();
	ConstantBuffer::PerMesh::s_cb.Reset();
	ConstantBuffer::PerCamera::s_cb.Reset();
	ConstantBuffer::PerFrame::s_cb.Reset();

	// 收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE SHADERS AND INPUT LAYOUTS 收收收收收收收收收收收收收收收收收收收收收收收收收
	ShaderComponent::VSSkyboxTransform::s_vs.Reset();
	ShaderComponent::VSStandardTransformP::s_vs.Reset();
	ShaderComponent::VSStandardTransformPC::s_vs.Reset();
	ShaderComponent::VSStandardTransformPN::s_vs.Reset();
	ShaderComponent::VSStandardTransformPT::s_vs.Reset();
	ShaderComponent::VSRenderingResultMerge::s_vs.Reset();
	ShaderComponent::VSStandardTransformPNT::s_vs.Reset();
	ShaderComponent::VSTerrainTransform::s_vs.Reset();

	ShaderComponent::HSTerrainRendering::s_hs.Reset();

	ShaderComponent::DSTerrainRendering::s_ds.Reset();

	ShaderComponent::PSSkyboxColoring::s_ps.Reset();
	ShaderComponent::PSStandardColoringP::s_ps.Reset();
	ShaderComponent::PSStandardColoringPC::s_ps.Reset();
	ShaderComponent::PSStandardColoringPN::s_ps.Reset();
	ShaderComponent::PSStandardColoringPT::s_ps.Reset();
	ShaderComponent::PSMSRenderingResultMerge::s_ps.Reset();
	ShaderComponent::PSStandardColoringPNT::s_ps.Reset();
	ShaderComponent::PSTerrainColoring::s_ps.Reset();

	for (size_t i = 0; i < static_cast<size_t>(VERTEX_FORMAT_TYPE::COUNT); ++i)
		InputLayout::s_items[i].Reset();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE CAMERA MERGER QUAD 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	CameraMergerQuadResource::Release();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE SKYBOX VB / IB 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	SkyboxResource::Release();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE SAMPLER STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	SamplerState::Release();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE DEPTHSTENCIL STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	DepthStencilState::Release();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE RASTERIZER STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	RasterizerState::Release();

	/*
	for (size_t i = 0; i < _countof(DeviceState::BlendState::s_items); ++i)
		DeviceState::BlendState::s_items[i].Reset();

	for (size_t i = 0; i < _countof(DeviceState::DepthStencilState::s_items); ++i)
		DeviceState::DepthStencilState::s_items[i].Reset();
	*/

	Graphics::s_dsvDepthStencilBuffer.Reset();
	Graphics::s_rtvColorBuffer.Reset();
	// 瞪羹�飛� 賅萄艘棻賊 蝶諜 羹檣 п薯 瞪縑 璽賅萄煎 滲唳п醜撿 觼楚衛 寞雖 陛棟
	{
		IDXGIOutput* pTarget = nullptr;
		BOOL isFullscreen;
		if (SUCCEEDED(Graphics::s_swapChain->GetFullscreenState(&isFullscreen, &pTarget)))
		{
			if (pTarget)
				pTarget->Release();
		}
		else
		{
			isFullscreen = FALSE;
		}

		// If not full screen, enable full screen again.
		if (isFullscreen)
			Graphics::s_swapChain->SetFullscreenState(FALSE, nullptr);
	}
	Graphics::s_swapChain.Reset();
	Graphics::s_immediateContext.Reset();
	Graphics::s_device.Reset();
}

void Graphics::EnableRendering()
{
	Graphics::s_flag &= ~Graphics::STATE_FLAG::RENDERING_DISABLED;
}

void Graphics::DisableRendering()
{
	Graphics::s_flag |= Graphics::STATE_FLAG::RENDERING_DISABLED;
}

bool Graphics::IsRenderingEnabled()
{
	return !(Graphics::s_flag & Graphics::STATE_FLAG::RENDERING_DISABLED);
}

UINT Graphics::CheckMSAAQuality(MSAACount sampleCount)
{
	UINT quality = 0;

	// first = Sample count
	// second = Quality level
	const size_t len = Graphics::s_msaa.size();
	for (const auto& info : Graphics::s_msaa)
	{
		if (info.first == sampleCount)
		{
			quality = info.second;
			break;
		}
	}

	return quality;
}

bool Graphics::QuerySupportedMSAAQuality()
{
	const MSAACount sc[6] = { MSAACount::OFF, MSAACount::X2, MSAACount::X4, MSAACount::X8, MSAACount::X16, MSAACount::X32 };
	HRESULT hr;

	for (UINT i = 0; i < _countof(sc); ++i)
	{
		UINT quality;
		hr = Graphics::GetDevice()->CheckMultisampleQualityLevels(BACKBUFFER_FORMAT, static_cast<UINT>(sc[i]), &quality);

		if (SUCCEEDED(hr) && quality != 0)
			Graphics::s_msaa.push_back(std::make_pair(sc[i], quality - 1));
	}
	
	return Graphics::s_msaa.size() > 0;
}

void Graphics::InitializeSwapChainAndDepthStencilBufferDescriptor()
{
	// Initialize SwapChain descriptor.
	Graphics::s_descSwapChain.BufferDesc.Width = Graphics::s_resolutionRequested.width;
	Graphics::s_descSwapChain.BufferDesc.Height = Graphics::s_resolutionRequested.height;
	Graphics::s_descSwapChain.BufferDesc.RefreshRate.Numerator = 60;
	Graphics::s_descSwapChain.BufferDesc.RefreshRate.Denominator = 1;
	Graphics::s_descSwapChain.BufferDesc.Format = BACKBUFFER_FORMAT;
	Graphics::s_descSwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	Graphics::s_descSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// (纔蝶お) 4X MSAA縑 maximum quailty蒂 堅薑戲煎 餌辨
	// The default sampler mode, with no anti-aliasing, has a count of 1 and a quality level of 0.
	Graphics::s_descSwapChain.SampleDesc.Count = static_cast<UINT>(MSAACount::X4);
	Graphics::s_descSwapChain.SampleDesc.Quality = Graphics::CheckMSAAQuality(MSAACount::X4);	// Use max quality level

	Graphics::s_descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Graphics::s_descSwapChain.BufferCount = BACKBUFFER_COUNT;
	Graphics::s_descSwapChain.OutputWindow = Engine::GetMainWindowHandle();
	Graphics::s_descSwapChain.Windowed = !Graphics::s_resolutionRequested.fullscreen;
	Graphics::s_descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	Graphics::s_descSwapChain.Flags = SWAP_CHAIN_FLAG;

	// Depth/Stencil buffer (幗ぷ 觼晦朝 奩萄衛 蝶諜 羹檣曖 寥 幗ぷ諦 橾纂п撿 л)
	Graphics::s_descDepthStencil.Width = Graphics::s_descSwapChain.BufferDesc.Width;
	Graphics::s_descDepthStencil.Height = Graphics::s_descSwapChain.BufferDesc.Height;
	Graphics::s_descDepthStencil.MipLevels = 1;
	Graphics::s_descDepthStencil.ArraySize = 1;
	Graphics::s_descDepthStencil.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Must match with swap chain's MSAA setting!
	Graphics::s_descDepthStencil.SampleDesc = Graphics::s_descSwapChain.SampleDesc;

	Graphics::s_descDepthStencil.Usage = D3D11_USAGE_DEFAULT;
	Graphics::s_descDepthStencil.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	Graphics::s_descDepthStencil.CPUAccessFlags = 0;
	Graphics::s_descDepthStencil.MiscFlags = 0;
}

void Graphics::RequestResolution(uint32_t width, uint32_t height, bool fullscreen)
{
	Graphics::s_resolutionRequested.width = width;
	Graphics::s_resolutionRequested.height = height;
	Graphics::s_resolutionRequested.fullscreen = fullscreen;
}

void Graphics::CommitResolution()
{
	Graphics::s_flag |= Graphics::STATE_FLAG::RESIZE_REQUIRED;
}

void Graphics::HandleDevice()
{
	HRESULT hr;

	/*
	if (Graphics::s_flag & Graphics::STATE_FLAG::STATUS_OCCLUDED)
		if (Graphics::s_swapChain->Present(0, DXGI_PRESENT_TEST) != DXGI_STATUS_OCCLUDED)
			Graphics::s_flag &= ~Graphics::STATE_FLAG::STATUS_OCCLUDED;
	*/

	if (!(Graphics::s_flag & Graphics::STATE_FLAG::RESIZE_REQUIRED))
		return;

	if (Graphics::s_flag & Graphics::STATE_FLAG::RENDERING_DISABLED)
		return;

	/*
	This method resets any device context to the default settings.
	This sets
	1. all input / output resource slots
	2. shaders
	3. input layouts
	4. predications
	5. scissor rectangles
	6. depth - stencil state
	7. rasterizer state
	8. blend state
	9. sampler state
	10. viewports
	to NULL.
	The primitive topology is set to UNDEFINED.
	For a scenario where you would like to clear a list of commands recorded so far,
	call ID3D11DeviceContext::FinishCommandList and throw away the resulting ID3D11CommandList.
	Graphics::GetImmediateContext()->ClearState();
	*/

	/*
	https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
	[Remarks] MSDN 頂辨
	You can't resize a swap chain unless you release all outstanding references to its back buffers. <- 醞蹂

	You must release all of its 'direct' and 'indirect' references on the back buffers in order for ResizeBuffers to succeed.
	(霜蕾 霤褻)
	'Direct references' are held by the application after it calls AddRef on a resource.

	(除蕾 霤褻)
	'Indirect references' are held by views to a resource, binding a view of the resource to a device context,
	a command list that used the resource, a command list that used a view to that resource, 
	a command list that executed another command list that used the resource, and so on.
	*/
	// Unbound render targets + Release resources that depend on backbuffer.
	ComponentSystem::CameraManager::OnBeforeResize();		// Release all camera's rtv and dsv
	Graphics::s_rtvColorBuffer.Reset();
	Graphics::s_dsvDepthStencilBuffer.Reset();

	Graphics::InitializeSwapChainAndDepthStencilBufferDescriptor();

	if (Graphics::s_resolutionRequested.fullscreen && !Graphics::s_backBufferResolution.fullscreen)
		Graphics::s_swapChain->SetFullscreenState(TRUE, nullptr);	// Windowed -> Fullscreen
	else if (!Graphics::s_resolutionRequested.fullscreen && Graphics::s_backBufferResolution.fullscreen)
		Graphics::s_swapChain->SetFullscreenState(FALSE, nullptr);	// Fullscreen -> Windowed
	
	// Graphics::s_swapChain->ResizeTarget();
	hr = Graphics::s_swapChain->ResizeBuffers(
		0,							// Set this number to zero to preserve the existing number of buffers in the swap chain.
		Graphics::s_resolutionRequested.width,
		Graphics::s_resolutionRequested.height,
		DXGI_FORMAT_UNKNOWN,		// Set this value to DXGI_FORMAT_UNKNOWN to preserve the existing format of the back buffer.
		SWAP_CHAIN_FLAG
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	// Update current back buffer resolution.
	Graphics::s_backBufferResolution = Graphics::s_resolutionRequested;
	Graphics::s_fullScreenViewport.Width = static_cast<FLOAT>(Graphics::s_backBufferResolution.width);
	Graphics::s_fullScreenViewport.Height = static_cast<FLOAT>(Graphics::s_backBufferResolution.height);

	// In order to bind the back buffer to the output merger stage, create RenderTargetView to the back buffer.
	ComPtr<ID3D11Texture2D> backBuffer;
	hr = Graphics::s_swapChain->GetBuffer(
		0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	// 溶渦 顫啃 箔 營儅撩
	hr = Graphics::GetDevice()->CreateRenderTargetView(
		backBuffer.Get(), nullptr, Graphics::s_rtvColorBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	hr = Graphics::GetDevice()->CreateTexture2D(&Graphics::GetDepthStencilDescriptor(), nullptr, depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	hr = Graphics::GetDevice()->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, s_dsvDepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	ComponentSystem::CameraManager::OnResize();	// 癱艙 ч溺, D3D11 箔んお 掘褻羹 機等檜お

	Graphics::s_flag &= ~Graphics::STATE_FLAG::RESIZE_REQUIRED;	// Off NOT_RENDERABLE bit flag
}

void Graphics::RenderFrame()
{
	if (ComponentSystem::CameraManager::s_updateRequired)
		ComponentSystem::CameraManager::Update();

	if (Graphics::s_flag != Graphics::STATE_FLAG::FINE)
		return;

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// PerFrame 鼻熱幗ぷ 撲薑
	hr = Graphics::GetImmediateContext()->Map(
		ConstantBuffer::PerFrame::s_cb.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return;
	}

	{
		ConstantBuffer::PerFrame* pCBPerFrame = reinterpret_cast<ConstantBuffer::PerFrame*>(mapped.pData);

		const uint32_t dlCount = static_cast<uint32_t>(ComponentSystem::LightManager::GetDirectionalLightCount());
		const uint32_t plCount = static_cast<uint32_t>(ComponentSystem::LightManager::GetPointLightCount());
		const uint32_t slCount = static_cast<uint32_t>(ComponentSystem::LightManager::GetSpotLightCount());
		pCBPerFrame->cb_directionalLightCount = dlCount;
		pCBPerFrame->cb_pointLightCount = plCount;
		pCBPerFrame->cb_spotLightCount = slCount;

		assert(
			dlCount <= MAX_GLOBAL_LIGHT_COUNT &&
			plCount <= MAX_GLOBAL_LIGHT_COUNT &&
			slCount <= MAX_GLOBAL_LIGHT_COUNT
		);

		for (uint32_t i = 0; i < dlCount; ++i)
		{
			const DirectionalLight* pDL = ComponentSystem::LightManager::s_dl[i].get();

			pCBPerFrame->cb_dl[i].diffuse = pDL->m_diffuse;
			pCBPerFrame->cb_dl[i].ambient = pDL->m_ambient;
			pCBPerFrame->cb_dl[i].specular = pDL->m_specular;
			XMStoreFloat3(
				&pCBPerFrame->cb_dl[i].directionW,
				XMVector3Rotate(LIGHT_DIRECTION_LOCAL_AXIS, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat4A(&pDL->GetGameObject()->m_transform.m_rotation)))
			);
		}

		for (uint32_t i = 0; i < plCount; ++i)
		{
			const PointLight* pPL = ComponentSystem::LightManager::s_pl[i].get();

			pCBPerFrame->cb_pl[i].diffuse = pPL->m_diffuse;
			pCBPerFrame->cb_pl[i].ambient = pPL->m_ambient;
			pCBPerFrame->cb_pl[i].specular = pPL->m_specular;
			pCBPerFrame->cb_pl[i].positionW = pPL->GetGameObject()->m_transform.m_position;
			pCBPerFrame->cb_pl[i].range = pPL->m_range;
			pCBPerFrame->cb_pl[i].att = pPL->m_att;
		}

		for (uint32_t i = 0; i < slCount; ++i)
		{
			const SpotLight* pSL = ComponentSystem::LightManager::s_sl[i].get();

			pCBPerFrame->cb_sl[i].diffuse = pSL->m_diffuse;
			pCBPerFrame->cb_sl[i].ambient = pSL->m_ambient;
			pCBPerFrame->cb_sl[i].specular = pSL->m_specular;
			pCBPerFrame->cb_sl[i].positionW = pSL->GetGameObject()->m_transform.m_position;
			pCBPerFrame->cb_sl[i].range = pSL->m_range;
			XMStoreFloat3(
				&pCBPerFrame->cb_sl[i].directionW,
				XMVector3Rotate(LIGHT_DIRECTION_LOCAL_AXIS, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat4A(&pSL->GetGameObject()->m_transform.m_rotation)))
			);
			pCBPerFrame->cb_sl[i].spotExp = pSL->m_spotExp;
			pCBPerFrame->cb_sl[i].att = pSL->m_att;
		}

		const Terrain* pTerrain = SceneManager::GetCurrentScene()->m_terrain.get();
		if (pTerrain)
		{
			pCBPerFrame->cb_terrainTextureTiling = pTerrain->GetTextureScale();
			pCBPerFrame->cb_terrainCellSpacing = pTerrain->GetCellSpacing();
			pCBPerFrame->cb_terrainTexelSpacingU = pTerrain->GetTexelSpacingU();
			pCBPerFrame->cb_terrainTexelSpacingV = pTerrain->GetTexelSpacingV();
		}
	}

	Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerFrame::s_cb.Get(), 0);
	// в蹂煎 ж朝 樁檜渦縑 鼻熱幗ぷ 撮た
	Graphics::GetImmediateContext()->PSSetConstantBuffers(ConstantBuffer::PerFrame::SLOT_NUM, 1, ConstantBuffer::PerFrame::s_cb.GetAddressOf());
	Graphics::GetImmediateContext()->DSSetConstantBuffers(ConstantBuffer::PerFrame::SLOT_NUM, 1, ConstantBuffer::PerFrame::s_cb.GetAddressOf());
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收

	// 詭衛 螃粽薛お 溶渦葭
	Graphics::GetImmediateContext()->RSSetState(RasterizerState::GetState(RasterizerState::FILL_MODE::SOLID, RasterizerState::CULL_MODE::BACK).GetInterface());
	ID3D11SamplerState* const ss[] = { SamplerState::GetState(SamplerState::TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X).GetInterface() };
	Graphics::GetImmediateContext()->PSSetSamplers(0, 1, ss);
	Graphics::GetImmediateContext()->OMSetDepthStencilState(DepthStencilState::GetState(DepthStencilState::DEPTH_STENCIL_STATE_TYPE::NORMAL).GetInterface(), 0);
	Graphics::GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::GetImmediateContext()->CSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->GSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->DSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->HSSetShader(nullptr, nullptr, 0);
	for (const auto& camera : ComponentSystem::CameraManager::s_cameras)
	{
		if (camera->m_rtvColorBuffer == nullptr || camera->m_srvColorBuffer == nullptr || camera->m_dsvDepthStencilBuffer == nullptr)
			if (FAILED(camera->CreateBufferAndView()))
				continue;

		if (!camera->IsEnabled())
			continue;

		const GameObject* const pCamGameObj = camera->GetGameObject();

		camera->UpdateViewMatrix();
		// camera->UpdateProjectionMatrix(); 掩檜 衙 Щ溯歜 п還 в蹂朝 橈擠

		Graphics::GetImmediateContext()->ClearRenderTargetView(camera->m_rtvColorBuffer.Get(), reinterpret_cast<const FLOAT*>(&camera->GetBackgroundColor()));
		Graphics::GetImmediateContext()->ClearDepthStencilView(camera->m_dsvDepthStencilBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Graphics::GetImmediateContext()->RSSetViewports(1, &camera->m_viewport);
		Graphics::GetImmediateContext()->OMSetRenderTargets(
			1,
			camera->m_rtvColorBuffer.GetAddressOf(),
			camera->m_dsvDepthStencilBuffer.Get()
		);

		const XMMATRIX vp = XMLoadFloat4x4A(&camera->m_viewMatrix) * XMLoadFloat4x4A(&camera->m_projMatrix);

		// PerCamera 鼻熱幗ぷ 撲薑
		{
			XMFLOAT4A frustumPlanes[6];
			Graphics::ExtractFrustumPlanesInWorldSpace(vp, frustumPlanes);	// Map 衛除 還檜晦 嬪п 嘐葬 啗骯

			hr = Graphics::GetImmediateContext()->Map(
				ConstantBuffer::PerCamera::s_cb.Get(),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mapped
			);
			if (FAILED(hr))
			{
				hrlog(THIS_FILE_NAME, __LINE__, hr);
				continue;
			}

			ConstantBuffer::PerCamera* pCBPerCamera = reinterpret_cast<ConstantBuffer::PerCamera*>(mapped.pData);
			pCBPerCamera->cb_camPosW = pCamGameObj->m_transform.m_position;
			pCBPerCamera->cb_tessMinDist = camera->GetMinimumDistanceForTessellationToStart();
			pCBPerCamera->cb_tessMaxDist = camera->GetMaximumDistanceForTessellationToStart();
			pCBPerCamera->cb_minTessExponent = camera->GetMinimumTessellationExponent();
			pCBPerCamera->cb_maxTessExponent = camera->GetMaximumTessellationExponent();
			pCBPerCamera->cb_frustumPlane[0] = frustumPlanes[0];
			pCBPerCamera->cb_frustumPlane[1] = frustumPlanes[1];
			pCBPerCamera->cb_frustumPlane[2] = frustumPlanes[2];
			pCBPerCamera->cb_frustumPlane[3] = frustumPlanes[3];
			pCBPerCamera->cb_frustumPlane[4] = frustumPlanes[4];
			pCBPerCamera->cb_frustumPlane[5] = frustumPlanes[5];
			Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerCamera::s_cb.Get(), 0);
			// в蹂煎 ж朝 樁檜渦縑 鼻熱幗ぷ 撮た
			Graphics::GetImmediateContext()->VSSetConstantBuffers(ConstantBuffer::PerCamera::SLOT_NUM, 1, ConstantBuffer::PerCamera::s_cb.GetAddressOf());
			Graphics::GetImmediateContext()->HSSetConstantBuffers(ConstantBuffer::PerCamera::SLOT_NUM, 1, ConstantBuffer::PerCamera::s_cb.GetAddressOf());
			Graphics::GetImmediateContext()->PSSetConstantBuffers(ConstantBuffer::PerCamera::SLOT_NUM, 1, ConstantBuffer::PerCamera::s_cb.GetAddressOf());
		}

		auto destroyedBegin = std::remove_if(
			ComponentSystem::MeshRendererManager::s_meshRenderers.begin(),
			ComponentSystem::MeshRendererManager::s_meshRenderers.end(),
			[&vp](const std::shared_ptr<MeshRenderer>& mr)
			{
				if (mr->IsDestroyed())
					return true;

				if (!mr->IsEnabled() || mr->m_mesh == nullptr)
					return false;

				switch (mr->m_mesh->GetVertexFormatType())
				{
				case VERTEX_FORMAT_TYPE::POSITION:
					StandardPEffect::Render(vp, mr.get());
					break;
				case VERTEX_FORMAT_TYPE::POSITION_COLOR:
					StandardPCEffect::Render(vp, mr.get());
					break;
				case VERTEX_FORMAT_TYPE::POSITION_NORMAL:
					StandardPNEffect::Render(vp, mr.get());
					break;
				case VERTEX_FORMAT_TYPE::POSITION_TEXCOORD:
					StandardPTEffect::Render(vp, mr.get());
					break;
				case VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD:
					StandardPNTEffect::Render(vp, mr.get());
					break;
				case VERTEX_FORMAT_TYPE::COUNT:
				case VERTEX_FORMAT_TYPE::UNKNOWN:
					FATAL_ERROR;
					break;
				}

				return false;
			}
		);

		ComponentSystem::MeshRendererManager::s_meshRenderers.erase(
			destroyedBegin,
			ComponentSystem::MeshRendererManager::s_meshRenderers.cend()
		);

		const XMVECTOR camPos = XMLoadFloat3A(&pCamGameObj->m_transform.m_position);
		// Graphics::GetImmediateContext()->RSSetState(RasterizerState::GetState(RasterizerState::FILL_MODE::WIREFRAME, RasterizerState::CULL_MODE::BACK).GetInterface());
		// 雖⑽ 溶渦葭
		const auto pTerrain = SceneManager::GetCurrentScene()->m_terrain.get();
		if (pTerrain)
			TerrainEffect::Render(vp, camPos, pTerrain);
		// Graphics::GetImmediateContext()->RSSetState(RasterizerState::GetState(RasterizerState::FILL_MODE::SOLID, RasterizerState::CULL_MODE::BACK).GetInterface());
		
		// 葆雖虞縑朝 蝶蘋檜夢蝶 溶渦葭
		const auto pSkybox = SceneManager::GetCurrentScene()->m_skybox.get();
		if (pSkybox)
			SkyboxEffect::Render(vp, camPos, pSkybox);
	}
	
	// 蘋詭塭 溶渦葭 唸婁 煽м
	Graphics::GetImmediateContext()->ClearRenderTargetView(Graphics::s_rtvColorBuffer.Get(), DirectX::Colors::Black);
	Graphics::GetImmediateContext()->ClearDepthStencilView(Graphics::s_dsvDepthStencilBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	Graphics::GetImmediateContext()->OMSetDepthStencilState(DepthStencilState::GetState(DepthStencilState::DEPTH_STENCIL_STATE_TYPE::CAMERA_MERGE).GetInterface(), 0);
	Graphics::GetImmediateContext()->RSSetViewports(1, &Graphics::s_fullScreenViewport);	// 瞪羹 寥幗ぷ縑 渠и 箔んお 撲薑
	Graphics::GetImmediateContext()->OMSetRenderTargets(
		1,
		Graphics::s_rtvColorBuffer.GetAddressOf(),
		Graphics::s_dsvDepthStencilBuffer.Get()
	);

	const UINT cmvbStride[] = { sizeof(VertexFormat::PositionTexCoord) };
	const UINT cmvbOffset[] = { 0 };
	ID3D11Buffer* const cmvb[] = { CameraMergerQuadResource::s_vb.Get() };
	Graphics::GetImmediateContext()->IASetVertexBuffers(0, 1, cmvb, cmvbStride, cmvbOffset);
	Graphics::GetImmediateContext()->IASetInputLayout(InputLayout::GetInputLayout(VERTEX_FORMAT_TYPE::POSITION_TEXCOORD));
	Graphics::GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// 鳴陝⑽ 蝶お董戲煎 撲薑!
	Graphics::GetImmediateContext()->VSSetShader(ShaderComponent::VSRenderingResultMerge::GetInterface(), nullptr, 0);
	Graphics::GetImmediateContext()->CSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->GSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->DSSetShader(nullptr, nullptr, 0);
	Graphics::GetImmediateContext()->HSSetShader(nullptr, nullptr, 0);
	// 忙式蘋詭塭 闡ん凱お 撲薑縑 評塭憮 а撚樁檜渦 殖塭螳撿 л! (MSAA 噙朝 蘋詭塭檣雖 嬴棋雖)
	Graphics::GetImmediateContext()->PSSetShader(ShaderComponent::PSMSRenderingResultMerge::GetInterface(), nullptr, 0);
	for (const auto& camera : ComponentSystem::CameraManager::s_cameras)
	{
		if (!camera->IsEnabled())
			continue;

		hr = Graphics::GetImmediateContext()->Map(
			ConstantBuffer::PerRenderingResultMerge::s_cb.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
			continue;
		}

		ConstantBuffer::PerRenderingResultMerge* pCBRndRstMerge = reinterpret_cast<ConstantBuffer::PerRenderingResultMerge*>(mapped.pData);
		pCBRndRstMerge->cb_width = camera->m_nzdvp.w;
		pCBRndRstMerge->cb_height = camera->m_nzdvp.h;
		pCBRndRstMerge->cb_topLeftX = camera->m_nzdvp.x;
		pCBRndRstMerge->cb_topLeftY = camera->m_nzdvp.y;
		Graphics::GetImmediateContext()->Unmap(ConstantBuffer::PerRenderingResultMerge::s_cb.Get(), 0);
		Graphics::GetImmediateContext()->VSSetConstantBuffers(ConstantBuffer::PerRenderingResultMerge::SLOT_NUM, 1, ConstantBuffer::PerRenderingResultMerge::s_cb.GetAddressOf());

		ID3D11ShaderResourceView* const cmvbsrv[] = { camera->m_srvColorBuffer.Get() };
		Graphics::GetImmediateContext()->PSSetShaderResources(TextureSlotNumber::RENDERING_RESULT_TEX_SLOT_NUM, 1, cmvbsrv);		// register(t0)

		Graphics::GetImmediateContext()->Draw(4, 0);
	}

	hr = Graphics::s_swapChain->Present(0, 0);

	if (hr == DXGI_STATUS_OCCLUDED)
		Graphics::s_flag |= Graphics::STATE_FLAG::STATUS_OCCLUDED;
	else if (FAILED(hr))
		hrlog(THIS_FILE_NAME, __LINE__, hr);
}

std::vector<byte> Graphics::LoadShaderByteCode(const wchar_t* path)
{
	std::vector<byte> byteCode;

	errno_t e;
	wchar_t filePath[MAX_PATH];
	filePath[0] = L'\0';

	do
	{
		// Open OBJ file
		e = wcscpy_s(filePath, FileSystem::GetResourcePath());
		if (e != 0)
		{
			LogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
			StringCbPrintfW(pLogBuffer->buffer, LogBuffer::BUFFER_SIZE, L"Failed to copy resource path.\n");
			AsyncFileLogger::PushLog(pLogBuffer);
			break;
		}

		e = wcscat_s(filePath, path);
		if (e != 0)
		{
			LogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
			StringCbPrintfW(pLogBuffer->buffer, LogBuffer::BUFFER_SIZE, L"Failed to copy resource path.\n");
			AsyncFileLogger::PushLog(pLogBuffer);
			break;
		}

		FILE* csoFile = nullptr;
		e = _wfopen_s(&csoFile, filePath, L"rb");
		if (e != 0 || !csoFile)
		{
			LogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
			StringCbPrintfW(pLogBuffer->buffer, LogBuffer::BUFFER_SIZE, L"Failed to open cso file.\n");
			AsyncFileLogger::PushLog(pLogBuffer);
			break;
		}

		fseek(csoFile, 0, SEEK_END);
		const uint32_t fileSize = static_cast<uint32_t>(ftell(csoFile));
		fseek(csoFile, 0, SEEK_SET);

		byteCode.resize(fileSize);

		const uint32_t bytesRead = static_cast<uint32_t>(fread_s(byteCode.data(), byteCode.size(), sizeof(byte), fileSize, csoFile));

		// Check error
		if (bytesRead != static_cast<size_t>(fileSize))
		{
			LogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
			StringCbPrintfW(pLogBuffer->buffer, LogBuffer::BUFFER_SIZE,
				L"Failed to read a cso file. bytes read: %u, file size: %u\n", bytesRead, fileSize);
			AsyncFileLogger::PushLog(pLogBuffer);
			break;
		}

		fclose(csoFile);

	} while (false);

	return byteCode;
}

void Graphics::ExtractFrustumPlanesInWorldSpace(XMMATRIX vp, XMFLOAT4A planes[6])
{
	XMFLOAT4X4A xm4x4flt;
	XMStoreFloat4x4A(&xm4x4flt, vp);

	// Left
	planes[0].x = xm4x4flt(0, 0) + xm4x4flt(0, 3);
	planes[0].y = xm4x4flt(1, 0) + xm4x4flt(1, 3);
	planes[0].z = xm4x4flt(2, 0) + xm4x4flt(2, 3);
	planes[0].w = xm4x4flt(3, 0) + xm4x4flt(3, 3);

	// Right
	planes[1].x = xm4x4flt(0, 3) - xm4x4flt(0, 0);
	planes[1].y = xm4x4flt(1, 3) - xm4x4flt(1, 0);
	planes[1].z = xm4x4flt(2, 3) - xm4x4flt(2, 0);
	planes[1].w = xm4x4flt(3, 3) - xm4x4flt(3, 0);

	// Bottom
	planes[2].x = xm4x4flt(0, 1) + xm4x4flt(0, 3);
	planes[2].y = xm4x4flt(1, 1) + xm4x4flt(1, 3);
	planes[2].z = xm4x4flt(2, 1) + xm4x4flt(2, 3);
	planes[2].w = xm4x4flt(3, 1) + xm4x4flt(3, 3);

	// Top
	planes[3].x = xm4x4flt(0, 3) - xm4x4flt(0, 1);
	planes[3].y = xm4x4flt(1, 3) - xm4x4flt(1, 1);
	planes[3].z = xm4x4flt(2, 3) - xm4x4flt(2, 1);
	planes[3].w = xm4x4flt(3, 3) - xm4x4flt(3, 1);

	// Near
	planes[4].x = xm4x4flt(0, 2);
	planes[4].y = xm4x4flt(1, 2);
	planes[4].z = xm4x4flt(2, 2);
	planes[4].w = xm4x4flt(3, 2);

	// Far
	planes[5].x = xm4x4flt(0, 3) - xm4x4flt(0, 2);
	planes[5].y = xm4x4flt(1, 3) - xm4x4flt(1, 2);
	planes[5].z = xm4x4flt(2, 3) - xm4x4flt(2, 2);
	planes[5].w = xm4x4flt(3, 3) - xm4x4flt(3, 2);

	for (uint32_t i = 0; i < 6; ++i)
	{
		const XMVECTOR v = XMPlaneNormalize(XMLoadFloat4A(&planes[i]));
		XMStoreFloat4A(&planes[i], v);
	}
}

Screen::Screen()
{
}

uint32_t Screen::GetWidth()
{
	return Graphics::GetBackBufferWidth();
}

uint32_t Screen::GetHeight()
{
	return Graphics::GetBackBufferHeight();
}

bool Screen::IsFullscreen()
{
	return Graphics::IsWindowFullscreen();
}

void Screen::SetResolution(uint32_t width, uint32_t height, bool fullscreen)
{
	if (Screen::GetWidth() == width &&
		Screen::GetHeight() == height &&
		Screen::IsFullscreen() == fullscreen)
		return;

	Graphics::RequestResolution(width, height, fullscreen);
	Graphics::CommitResolution();
}

int DepthStencilState::Init()
{
	HRESULT hr;

	D3D11_DEPTH_STENCIL_DESC descDepthStencil;
	ZeroMemory(&descDepthStencil, sizeof(descDepthStencil));

	// 1. 橾奩 溶渦葭辨
	descDepthStencil.DepthEnable = TRUE;
	descDepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.DepthFunc = D3D11_COMPARISON_LESS;
	descDepthStencil.StencilEnable = FALSE;	// 橾奩 溶渦葭辨
	hr = Graphics::GetDevice()->CreateDepthStencilState(
		&descDepthStencil, DepthStencilState::s_items[static_cast<size_t>(DepthStencilState::DEPTH_STENCIL_STATE_TYPE::NORMAL)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	// 2. 蝶蘋檜夢蝶 溶渦葭辨
	descDepthStencil.DepthEnable = TRUE;
	descDepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	// 蝶蘋檜夢蝶 溶渦葭 衛 醞蹂
	descDepthStencil.StencilEnable = FALSE;
	hr = Graphics::GetDevice()->CreateDepthStencilState(
		&descDepthStencil, DepthStencilState::s_items[static_cast<size_t>(DepthStencilState::DEPTH_STENCIL_STATE_TYPE::SKYBOX)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	// 3. 剪選 溶渦葭辨

	// 4. 蘋詭塭 煽м辨
	ZeroMemory(&descDepthStencil, sizeof(descDepthStencil));
	descDepthStencil.DepthEnable = FALSE;
	descDepthStencil.StencilEnable = FALSE;
	hr = Graphics::GetDevice()->CreateDepthStencilState(
		&descDepthStencil, DepthStencilState::s_items[static_cast<size_t>(DepthStencilState::DEPTH_STENCIL_STATE_TYPE::CAMERA_MERGE)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	return 0;
}

void DepthStencilState::Release()
{
	for (size_t i = 0; i < static_cast<size_t>(DepthStencilState::DEPTH_STENCIL_STATE_TYPE::COUNT); ++i)
		DepthStencilState::s_items[i].m_interface.Reset();
}

int RasterizerState::Init()
{
	HRESULT hr;

	D3D11_RASTERIZER_DESC descRS;
	ZeroMemory(&descRS, sizeof(descRS));
	descRS.MultisampleEnable = TRUE;

	descRS.FillMode = D3D11_FILL_WIREFRAME;
	descRS.CullMode = D3D11_CULL_NONE;
	hr = Graphics::GetDevice()->CreateRasterizerState(
		&descRS,
		RasterizerState::s_items[static_cast<size_t>(RasterizerState::FILL_MODE::WIREFRAME)][static_cast<size_t>(RasterizerState::CULL_MODE::NONE)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	descRS.FillMode = D3D11_FILL_WIREFRAME;
	descRS.CullMode = D3D11_CULL_FRONT;
	hr = Graphics::GetDevice()->CreateRasterizerState(
		&descRS,
		RasterizerState::s_items[static_cast<size_t>(RasterizerState::FILL_MODE::WIREFRAME)][static_cast<size_t>(RasterizerState::CULL_MODE::FRONT)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	descRS.FillMode = D3D11_FILL_WIREFRAME;
	descRS.CullMode = D3D11_CULL_BACK;
	hr = Graphics::GetDevice()->CreateRasterizerState(
		&descRS,
		RasterizerState::s_items[static_cast<size_t>(RasterizerState::FILL_MODE::WIREFRAME)][static_cast<size_t>(RasterizerState::CULL_MODE::BACK)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	descRS.FillMode = D3D11_FILL_SOLID;
	descRS.CullMode = D3D11_CULL_NONE;
	hr = Graphics::GetDevice()->CreateRasterizerState(
		&descRS,
		RasterizerState::s_items[static_cast<size_t>(RasterizerState::FILL_MODE::SOLID)][static_cast<size_t>(RasterizerState::CULL_MODE::NONE)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	descRS.FillMode = D3D11_FILL_SOLID;
	descRS.CullMode = D3D11_CULL_FRONT;
	hr = Graphics::GetDevice()->CreateRasterizerState(
		&descRS,
		RasterizerState::s_items[static_cast<size_t>(RasterizerState::FILL_MODE::SOLID)][static_cast<size_t>(RasterizerState::CULL_MODE::FRONT)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	descRS.FillMode = D3D11_FILL_SOLID;
	descRS.CullMode = D3D11_CULL_BACK;
	hr = Graphics::GetDevice()->CreateRasterizerState(
		&descRS,
		RasterizerState::s_items[static_cast<size_t>(RasterizerState::FILL_MODE::SOLID)][static_cast<size_t>(RasterizerState::CULL_MODE::BACK)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	return 0;
}

void RasterizerState::Release()
{
	for (size_t i = 0; i < static_cast<size_t>(RasterizerState::FILL_MODE::COUNT); ++i)
		for (size_t j = 0; j < static_cast<size_t>(RasterizerState::CULL_MODE::COUNT); ++j)
			RasterizerState::s_items[i][j].m_interface.Reset();
}

int SkyboxResource::Init()
{
	HRESULT hr;
	D3D11_BUFFER_DESC descBuffer;
	ZeroMemory(&descBuffer, sizeof(descBuffer));

	descBuffer.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	descBuffer.CPUAccessFlags = 0;
	descBuffer.MiscFlags = 0;
	descBuffer.StructureByteStride = 0;

	descBuffer.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	descBuffer.ByteWidth = sizeof(VertexFormat::Position) * 8;
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	const VertexFormat::Position skyboxVertices[8] =
	{
		XMFLOAT3(-1.0f, -1.0f, -1.0f),
		XMFLOAT3(-1.0f, -1.0f, +1.0f),
		XMFLOAT3(-1.0f, +1.0f, -1.0f),
		XMFLOAT3(-1.0f, +1.0f, +1.0f),
		XMFLOAT3(+1.0f, -1.0f, -1.0f),
		XMFLOAT3(+1.0f, -1.0f, +1.0f),
		XMFLOAT3(+1.0f, +1.0f, -1.0f),
		XMFLOAT3(+1.0f, +1.0f, +1.0f),
	};
	vertexData.pSysMem = skyboxVertices;

	hr = Graphics::GetDevice()->CreateBuffer(&descBuffer, &vertexData, SkyboxResource::s_vb.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	descBuffer.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	descBuffer.ByteWidth = sizeof(index_format) * 36;
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	const index_format skyboxIndices[36] =
	{
		0, 2, 1,
		1, 2, 3,

		2, 6, 3,
		3, 6, 7,

		6, 4, 5,
		6, 5, 7,

		4, 0, 1,
		4, 1, 5,

		0, 4, 6,
		0, 6, 2,

		1, 3, 7,
		1, 7, 5
	};
	indexData.pSysMem = skyboxIndices;
	hr = Graphics::GetDevice()->CreateBuffer(&descBuffer, &indexData, SkyboxResource::s_ib.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	return 0;
}

void SkyboxResource::Release()
{
	SkyboxResource::s_vb.Reset();
	SkyboxResource::s_ib.Reset();
}

int CameraMergerQuadResource::Init()
{
	// LEFT BOTTOM
	VertexFormat::PositionTexCoord vertex[4];
	vertex[0].m_position = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertex[0].m_texCoord = XMFLOAT2(0.0f, 1.0f);
	// LEFT TOP
	vertex[1].m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].m_texCoord = XMFLOAT2(0.0f, 0.0f);
	// RIGHT BOTTOM
	vertex[2].m_position = XMFLOAT3(+1.0f, -1.0f, 0.0f);
	vertex[2].m_texCoord = XMFLOAT2(1.0f, 1.0f);
	// RIGHT UP
	vertex[3].m_position = XMFLOAT3(+1.0f, 0.0f, 0.0f);
	vertex[3].m_texCoord = XMFLOAT2(1.0f, 0.0f);

	D3D11_BUFFER_DESC descBuffer;
	ZeroMemory(&descBuffer, sizeof(descBuffer));

	// 鼻熱 幗ぷ 奢鱔 樓撩 撮た
	descBuffer.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	descBuffer.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	descBuffer.ByteWidth = sizeof(VertexFormat::PositionTexCoord) * 4;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertex;

	HRESULT hr = Graphics::GetDevice()->CreateBuffer(&descBuffer, &initData, CameraMergerQuadResource::s_vb.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	return 0;
}

void CameraMergerQuadResource::Release()
{
	CameraMergerQuadResource::s_vb.Reset();
}

int SamplerState::Init()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC descSampler;
	
	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	hr = Graphics::GetDevice()->CreateSamplerState(
		&descSampler,
		SamplerState::s_items[static_cast<uint32_t>(SamplerState::TEXTURE_FILTERING_OPTION::POINT)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; // 誰摹⑽ (Bilinear)
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::GetDevice()->CreateSamplerState(
		&descSampler,
		SamplerState::s_items[static_cast<uint32_t>(SamplerState::TEXTURE_FILTERING_OPTION::BILINEAR)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // 鳴摹⑽ (Trilinear)
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::GetDevice()->CreateSamplerState(
		&descSampler,
		SamplerState::s_items[static_cast<uint32_t>(SamplerState::TEXTURE_FILTERING_OPTION::TRILINEAR)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_ANISOTROPIC;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MaxAnisotropy = 2;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::GetDevice()->CreateSamplerState(
		&descSampler,
		SamplerState::s_items[static_cast<uint32_t>(SamplerState::TEXTURE_FILTERING_OPTION::ANISOTROPIC_2X)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	descSampler.MaxAnisotropy = 4;
	Graphics::GetDevice()->CreateSamplerState(
		&descSampler,
		SamplerState::s_items[static_cast<uint32_t>(SamplerState::TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	descSampler.MaxAnisotropy = 8;
	Graphics::GetDevice()->CreateSamplerState(
		&descSampler,
		SamplerState::s_items[static_cast<uint32_t>(SamplerState::TEXTURE_FILTERING_OPTION::ANISOTROPIC_8X)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	descSampler.MaxAnisotropy = 16;
	Graphics::GetDevice()->CreateSamplerState(
		&descSampler,
		SamplerState::s_items[static_cast<uint32_t>(SamplerState::TEXTURE_FILTERING_OPTION::ANISOTROPIC_16X)].m_interface.GetAddressOf()
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	return 0;
}

void SamplerState::Release()
{
	for (size_t i = 0; i < static_cast<size_t>(SamplerState::TEXTURE_FILTERING_OPTION::COUNT); ++i)
		SamplerState::s_items[i].m_interface.Reset();
}
