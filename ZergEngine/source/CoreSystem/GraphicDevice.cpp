#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\RenderState.h>

namespace ze
{
	GraphicDeviceImpl GraphicDevice;
}

using namespace ze;

PCWSTR SHADER_PATH = L"Engine\\Bin\\Shader\\";

static PCWSTR VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::COUNT)] =
{
	L"VSTransformSkyboxToHCS.cso",
	L"VSTransformTerrainPatchCtrlPt.cso",
	L"VSTransformPToHCS.cso",
	L"VSTransformPCToHCS.cso",
	L"VSTransformPNToHCS.cso",
	L"VSTransformPTToHCS.cso",
	L"VSTransformCameraMergeQuad.cso",
	L"VSTransformPNTToHCS.cso",
	L"VSTransformButtonToHCS.cso"
};
static PCWSTR HULL_SHADER_FILES[static_cast<size_t>(HULL_SHADER_TYPE::COUNT)] =
{
	L"HSCalcTerrainTessFactor.cso"
};
static PCWSTR DOMAIN_SHADER_FILES[static_cast<size_t>(DOMAIN_SHADER_TYPE::COUNT)] =
{
	L"DSSampleTerrainHeightMap.cso"
};
static PCWSTR PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COUNT)] =
{
	L"PSColorSkyboxFragment.cso",
	L"PSColorTerrainFragment.cso",
	L"PSColorPFragment.cso",
	L"PSColorPCFragment.cso",
	L"PSColorPNFragment.cso",
	L"PSColorPTFragment.cso",
	L"PSColorPNTFragment.cso",
	L"PSColorPTFragmentSingleTexture.cso",		// unimplemented
	L"PSColorPTFragmentSingleMSTexture.cso",
	L"PSColorButtonFragment.cso"
};

// constexpr float allows only one floating point constant to exist in memory, even if it is not encoded in a x86 command.
constexpr DXGI_FORMAT BACKBUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr uint32_t SWAP_CHAIN_FLAG = 0;

GraphicDeviceImpl::GraphicDeviceImpl()
	: m_descAdapter()
	, m_descSwapChain()
	, m_descDepthStencil()
	, m_fullSwapChainViewport()
	, m_pDevice(nullptr)
	, m_pImmediateContext(nullptr)
	, m_supportedResolution()
	, m_supportedMSAA()
	, m_pSwapChain(nullptr)
	, m_pSwapChainRTV(nullptr)
	, m_pSwapChainDSV(nullptr)
	, m_vs{}
	, m_hs{}
	, m_ds{}
	, m_ps{}
	, m_il{}
	, m_vb{}
	, m_rs{}
	, m_ss{}
	, m_ssSkybox()
	, m_ssHeightMap()
	, m_dss{}
	, m_bs{}
{
}

GraphicDeviceImpl::~GraphicDeviceImpl()
{
}

void GraphicDeviceImpl::Init(void* pDesc)
{
	HRESULT hr;

#if defined(DEBUG) || defined(_DEBUG)
	const UINT createDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#else
	const UINT createDeviceFlag = 0;
#endif
	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,	// For the Direct2D interoperability with Direc3D 11
	};
	D3D_FEATURE_LEVEL maxSupportedFeatureLevel;

	assert(m_pDevice == nullptr);
	assert(m_pImmediateContext == nullptr);
	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlag,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		&m_pDevice,
		&maxSupportedFeatureLevel,
		&m_pImmediateContext
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"D3D11CreateDevice()", hr);

	if (maxSupportedFeatureLevel < D3D_FEATURE_LEVEL_11_0)
		Debug::ForceCrashWithMessageBox(L"Fail", L"Device does not support D3D11 feature level.");

	// Áö¿øµÇ´Â ÇØ»óµµ ¸ñ·Ï °Ë»ö
	this->CreateSupportedResolutionInfo();

	// ÇÏµå¿þ¾î¿¡¼­ Áö¿øÇÏ´Â ¸ÖÆ¼»ùÇÃ¸µ Ä«¿îÆ® Ã¼Å©
	// FEATURE_LEVEL_11_0 devices are required to support 4x MSAA for all render target formats,
	// and 8x MSAA for all render target formats except R32G32B32A32 formats.
	this->CreateSupportedMSAAQualityInfo();

	this->InitializeSwapChainAndDepthStencilBufferDesc();

	// ½º¿Ò Ã¼ÀÎ »ý¼º
	// First, Get IDXGIFactory instance
	// DirectX Graphics Infrastructure(DXGI)
	ComPtr<IDXGIDevice> cpDXGIDevice;
	ComPtr<IDXGIAdapter> cpDXGIAdapter;
	ComPtr<IDXGIFactory> cpDXGIFactory;

	// Use the IDXGIFactory instance that was used to create the device! (by COM queries)
	do
	{
		hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(cpDXGIDevice.GetAddressOf()));
		if (FAILED(hr))
			break;

		hr = cpDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(cpDXGIAdapter.GetAddressOf()));
		if (FAILED(hr))
			break;

		// ¾î´ðÅÍ Á¤º¸ È¹µæ
		hr = cpDXGIAdapter->GetDesc(&m_descAdapter);
		if (FAILED(hr))
			break;

		hr = cpDXGIAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(cpDXGIFactory.GetAddressOf()));
		if (FAILED(hr))
			break;

		// Create a swap chain.
		assert(m_pSwapChain == nullptr);
		hr = cpDXGIFactory->CreateSwapChain(m_pDevice, &m_descSwapChain, &m_pSwapChain);
		if (FAILED(hr))
			break;

		// ½º¿Ò Ã¼ÀÎÀÇ ¹é¹öÆÛ¿¡ ´ëÇÑ ·»´õ Å¸°Ù ºä Àç»ý¼º
		ComPtr<ID3D11Texture2D> cpBackBuffer;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(cpBackBuffer.GetAddressOf()));
		if (FAILED(hr))
			break;

		assert(m_pSwapChainRTV == nullptr);
		hr = m_pDevice->CreateRenderTargetView(cpBackBuffer.Get(), nullptr, &m_pSwapChainRTV);
		if (FAILED(hr))
			break;

		// ½º¿Ò Ã¼ÀÎ¿¡ ´ëÇÑ ·»´õ¸µ¿¡ »ç¿ëÇÒ µª½º ½ºÅÙ½Ç ºä Àç»ý¼º
		ComPtr<ID3D11Texture2D> cpDepthStencilBuffer;
		hr = m_pDevice->CreateTexture2D(&m_descDepthStencil, nullptr, cpDepthStencilBuffer.GetAddressOf());
		if (FAILED(hr))
			break;

		assert(m_pSwapChainDSV == nullptr);
		hr = m_pDevice->CreateDepthStencilView(cpDepthStencilBuffer.Get(), nullptr, &m_pSwapChainDSV);
		if (FAILED(hr))
			break;
	} while (false);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Failed to create a swap chain.", hr);

	// Detect if newly created full-screen swap chain isn't actually full screen.
	// https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgifactory-createswapchain
	// Remark Âü°í
	// ÀüÃ¼È­¸éÀ¸·Î ½º¿Ò Ã¼ÀÎÀ» »ý¼ºÇÏ°í Present¸¦ È£ÃâÇÒ ¶§ ·±Å¸ÀÓÀÌ ¿¹±âÄ¡ ¾Ê°Ô ÀüÃ¼È­¸éÀ» Á¾·áÇÒ ¼ö ÀÖ´Â ¹®Á¦¸¦ ÇØ°áÇÏ´Â ÄÚµå.
	if (m_descSwapChain.Windowed == FALSE)
	{
		BOOL isFullscreen;
		hr = this->GetFullscreenState(&isFullscreen);
		// If not full screen, enable full screen again.
		if (SUCCEEDED(hr) && isFullscreen == FALSE)
		{
			ShowWindow(Window.GetWindowHandle(), SW_MINIMIZE);
			ShowWindow(Window.GetWindowHandle(), SW_RESTORE);
			m_pSwapChain->SetFullscreenState(TRUE, nullptr);
		}
	}

	m_fullSwapChainViewport.TopLeftX = 0.0f;
	m_fullSwapChainViewport.TopLeftY = 0.0f;
	m_fullSwapChainViewport.Width = static_cast<float>(m_descSwapChain.BufferDesc.Width);
	m_fullSwapChainViewport.Height = static_cast<float>(m_descSwapChain.BufferDesc.Height);
	m_fullSwapChainViewport.MinDepth = 0.0f;
	m_fullSwapChainViewport.MaxDepth = 1.0f;

	// DXGI ÆíÀÇ±â´É ºñÈ°¼ºÈ­
	hr = cpDXGIFactory->MakeWindowAssociation(Window.GetWindowHandle(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"MakeWindowAssociation()", hr);

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ CREATE SHADERS AND INPUT LAYOUTS ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->CreateShaderAndInputLayout();

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ CREATE RASTERIZER STATES ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->CreateRasterizerStates();

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ CREATE SAMPLER STATES ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->CreateSamplerStates();

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ CREATE DEPTHSTENCIL STATES ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->CreateDepthStencilStates();

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ CREATE BLEND STATES ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->CreateBlendStates();

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ CREATE COMMON VERTEX BUFFERS ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->CreateCommonVertexBuffers();
}

void GraphicDeviceImpl::Release()
{
	HRESULT hr;

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ RELEASE COMMON VERTEX BUFFERS ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->ReleaseCommonVertexBuffers();

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ RELEASE BLEND STATES ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->ReleaseBlendStates();

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ RELEASE DEPTHSTENCIL STATES ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->ReleaseDepthStencilStates();

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ RELEASE SAMPLER STATES ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->ReleaseSamplerStates();

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ RELEASE RASTERIZER STATES ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->ReleaseRasterizerStates();

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ RELEASE SHADERS AND INPUT LAYOUTS ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->ReleaseShaderAndInputLayout();

	Helper::SafeReleaseCom(m_pSwapChainDSV);
	Helper::SafeReleaseCom(m_pSwapChainRTV);

	// ÀüÃ¼È­¸é ¸ðµå¿´´Ù¸é ½º¿Ò Ã¼ÀÎ ÇØÁ¦ Àü¿¡ Ã¢¸ðµå·Î º¯°æÇØÁà¾ß Å©·¡½Ã ¹æÁö °¡´É
	// https://learn.microsoft.com/ko-kr/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi#destroying-a-swap-chain
	// '½º¿Ò Ã¼ÀÎ »èÁ¦' Ç×¸ñ Âü°í
	if (m_pSwapChain != nullptr)
	{
		BOOL isFullscreen;
		hr = this->GetFullscreenState(&isFullscreen);
		if (SUCCEEDED(hr) && isFullscreen != FALSE)
			m_pSwapChain->SetFullscreenState(FALSE, nullptr);
	}

	m_supportedMSAA.clear();
	Helper::SafeReleaseCom(m_pSwapChain);
	Helper::SafeReleaseCom(m_pImmediateContext);
	Helper::SafeReleaseCom(m_pDevice);
}

bool GraphicDeviceImpl::LoadShaderByteCode(PCWSTR path, byte** ppByteCode, size_t* pSize)
{
	byte* pByteCode = nullptr;
	FILE* pCSOFile = nullptr;
	WCHAR filePath[MAX_PATH];

	HRESULT hr;
	hr = FileSystem.RelativePathToFullPath(path, filePath, sizeof(filePath));
	if (FAILED(hr))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to create full file path.\n%s", path);

	errno_t e = _wfopen_s(&pCSOFile, filePath, L"rb");
	if (e != 0)
		return false;

	Helper::AutoCRTFileCloser af(pCSOFile);

	fseek(pCSOFile, 0, SEEK_END);
	const size_t fileSize = static_cast<size_t>(ftell(pCSOFile));
	fseek(pCSOFile, 0, SEEK_SET);

	pByteCode = new byte[fileSize];

	const size_t bytesRead = static_cast<size_t>(fread_s(pByteCode, fileSize, 1, fileSize, pCSOFile));
	// Check error
	assert(bytesRead == fileSize);

	af.Close();

	// ¹ÙÀÌÆ® ÄÚµå¿Í Å©±â ¸®ÅÏ
	*ppByteCode = pByteCode;
	*pSize = fileSize;

	return true;
}

void GraphicDeviceImpl::CreateShaderAndInputLayout()
{
	ID3D11Device* pDevice = GraphicDevice.GetDeviceComInterface();

	byte* pByteCode = nullptr;
	size_t byteCodeSize = 0;
	WCHAR targetPath[MAX_PATH];

	// VERTEX SHADERS
	// 1. TRANSFORM_SKYBOX_TO_HCS (No Input Layout required)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_SKYBOX_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_SKYBOX_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 2. TRANSFORM_TERRAIN_PATCH_CTRL_PT (POSITION, TEXCOORD0, TEXCOORD1)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_TERRAIN_PATCH_CTRL_PT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_TERRAIN_PATCH_CTRL_PT)].Init(pDevice, pByteCode, byteCodeSize);
	// ¦±¦¬ Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::TERRAIN_PATCH_CTRL_PT)].Init(
		pDevice,
		VFTerrainPatchControlPoint::GetInputElementDescriptor(),
		VFTerrainPatchControlPoint::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 3. TRANSFORM_P_TO_HCS (POSITION)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_P_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_P_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ¦±¦¬ Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION)].Init(
		pDevice,
		VFPosition::GetInputElementDescriptor(),
		VFPosition::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 4. TRANSFORM_PC_TO_HCS (POSITION, COLOR0)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PC_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PC_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ¦±¦¬ Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_COLOR)].Init(
		pDevice,
		VFPositionColor::GetInputElementDescriptor(),
		VFPositionColor::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 5. TRANSFORM_PN_TO_HCS (POSITION, NORMAL)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PN_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PN_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ¦±¦¬ Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_NORMAL)].Init(
		pDevice,
		VFPositionNormal::GetInputElementDescriptor(),
		VFPositionNormal::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 6. TRANSFORM_PT_TO_HCS (POSITION, TEXCOORD0)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PT_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PT_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ¦±¦¬ Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_TEXCOORD)].Init(
		pDevice,
		VFPositionTexCoord::GetInputElementDescriptor(),
		VFPositionTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 7.TRANSFORM_CAMERA_MERGE_QUAD (No Input Layout required)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_CAMERA_MERGE_QUAD)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_CAMERA_MERGE_QUAD)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 8. TRANSFORM_PNT_TO_HCS (POSITION, NORMAL, TEXCOORD)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PNT_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PNT_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ¦±¦¬ Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD)].Init(
		pDevice,
		VFPositionNormalTexCoord::GetInputElementDescriptor(),
		VFPositionNormalTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 9. TRANSFORM_BUTTON_TO_HCS (POSITION, TEXCOORD0, COLOR0)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_BUTTON_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_BUTTON_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ¦±¦¬ Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::BUTTON)].Init(
		pDevice,
		VFButton::GetInputElementDescriptor(),
		VFButton::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// HULL SHADERS
	// 1. HS CALC_TERRAIN_TESS_FACTOR
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), HULL_SHADER_FILES[static_cast<size_t>(HULL_SHADER_TYPE::CALC_TERRAIN_TESS_FACTOR)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_hs[static_cast<size_t>(HULL_SHADER_TYPE::CALC_TERRAIN_TESS_FACTOR)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// DOMAIN SHADERS
	// 1. DS SAMPLE_TERRAIN_HEIGHT_MAP
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), DOMAIN_SHADER_FILES[static_cast<size_t>(DOMAIN_SHADER_TYPE::SAMPLE_TERRAIN_HEIGHT_MAP)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ds[static_cast<size_t>(DOMAIN_SHADER_TYPE::SAMPLE_TERRAIN_HEIGHT_MAP)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// PIXEL SHADERS
	// 1. COLOR_SKYBOX_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_SKYBOX_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_SKYBOX_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 2. COLOR_TERRAIN_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_TERRAIN_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_TERRAIN_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 3. COLOR_P_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_P_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_P_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 4. COLOR_PC_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PC_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PC_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 5. COLOR_PN_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PN_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PN_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 6. COLOR_PT_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 7. COLOR_PNT_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PNT_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PNT_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 8. COLOR_PT_FRAGMENT_SINGLE_TEXTURE (Not implemented)
	/*
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_TEXTURE)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_TEXTURE)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;
	*/

	// 9. COLOR_PT_FRAGMENT_SINGLE_MSTEXTURE
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_MSTEXTURE)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_MSTEXTURE)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 10. COLOR_BUTTON_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_BUTTON_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_BUTTON_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;
}

void GraphicDeviceImpl::ReleaseShaderAndInputLayout()
{
	for (size_t i = 0; i < _countof(m_vs); ++i)
		m_vs[i].Release();

	for (size_t i = 0; i < _countof(m_hs); ++i)
		m_hs[i].Release();

	for (size_t i = 0; i < _countof(m_ds); ++i)
		m_ds[i].Release();

	for (size_t i = 0; i < _countof(m_ps); ++i)
		m_ps[i].Release();

	for (size_t i = 0; i < _countof(m_il); ++i)
		m_il[i].Release();
}

void GraphicDeviceImpl::CreateRasterizerStates()
{
	ID3D11Device* pDevice = GraphicDevice.GetDeviceComInterface();

	D3D11_RASTERIZER_DESC descRS;
	ZeroMemory(&descRS, sizeof(descRS));
	descRS.MultisampleEnable = TRUE;

	descRS.FillMode = D3D11_FILL_WIREFRAME;
	descRS.CullMode = D3D11_CULL_NONE;
	m_rs[static_cast<size_t>(RASTERIZER_FILL_MODE::WIREFRAME)][static_cast<size_t>(RASTERIZER_CULL_MODE::NONE)].Init(
		pDevice, &descRS
	);

	descRS.FillMode = D3D11_FILL_WIREFRAME;
	descRS.CullMode = D3D11_CULL_FRONT;
	m_rs[static_cast<size_t>(RASTERIZER_FILL_MODE::WIREFRAME)][static_cast<size_t>(RASTERIZER_CULL_MODE::FRONT)].Init(
		pDevice, &descRS
	);

	descRS.FillMode = D3D11_FILL_WIREFRAME;
	descRS.CullMode = D3D11_CULL_BACK;
	m_rs[static_cast<size_t>(RASTERIZER_FILL_MODE::WIREFRAME)][static_cast<size_t>(RASTERIZER_CULL_MODE::BACK)].Init(
		pDevice, &descRS
	);

	descRS.FillMode = D3D11_FILL_SOLID;
	descRS.CullMode = D3D11_CULL_NONE;
	m_rs[static_cast<size_t>(RASTERIZER_FILL_MODE::SOLID)][static_cast<size_t>(RASTERIZER_CULL_MODE::NONE)].Init(
		pDevice, &descRS
	);

	descRS.FillMode = D3D11_FILL_SOLID;
	descRS.CullMode = D3D11_CULL_FRONT;
	m_rs[static_cast<size_t>(RASTERIZER_FILL_MODE::SOLID)][static_cast<size_t>(RASTERIZER_CULL_MODE::FRONT)].Init(
		pDevice, &descRS
	);

	descRS.FillMode = D3D11_FILL_SOLID;
	descRS.CullMode = D3D11_CULL_BACK;
	m_rs[static_cast<size_t>(RASTERIZER_FILL_MODE::SOLID)][static_cast<size_t>(RASTERIZER_CULL_MODE::BACK)].Init(
		pDevice, &descRS
	);
}

void GraphicDeviceImpl::ReleaseRasterizerStates()
{
	for (size_t i = 0; i < static_cast<size_t>(RASTERIZER_FILL_MODE::COUNT); ++i)
		for (size_t j = 0; j < static_cast<size_t>(RASTERIZER_CULL_MODE::COUNT); ++j)
			m_rs[i][j].Release();
}

void GraphicDeviceImpl::CreateSamplerStates()
{
	ID3D11Device* pDevice = GraphicDevice.GetDeviceComInterface();

	D3D11_SAMPLER_DESC descSampler;

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::POINT)].Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; // ½Ö¼±Çü (Bilinear)
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::BILINEAR)].Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // »ï¼±Çü (Trilinear)
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::TRILINEAR)].Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_ANISOTROPIC;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MaxAnisotropy = 2;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_2X)].Init(pDevice, &descSampler);

	descSampler.MaxAnisotropy = 4;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X)].Init(pDevice, &descSampler);

	descSampler.MaxAnisotropy = 8;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_8X)].Init(pDevice, &descSampler);

	descSampler.MaxAnisotropy = 16;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_16X)].Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_ssSkybox.Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_ssHeightMap.Init(pDevice, &descSampler);
}

void GraphicDeviceImpl::ReleaseSamplerStates()
{
	for (size_t i = 0; i < _countof(m_ss); ++i)
		m_ss[i].Release();

	m_ssSkybox.Release();
	m_ssHeightMap.Release();
}

void GraphicDeviceImpl::CreateDepthStencilStates()
{
	ID3D11Device* pDevice = GraphicDevice.GetDeviceComInterface();

	D3D11_DEPTH_STENCIL_DESC descDepthStencil;
	ZeroMemory(&descDepthStencil, sizeof(descDepthStencil));

	// 1. Default
	// m_dss[static_cast<size_t>(DEPTH_STENCIL_STATETYPE::DEFAULT)].Init();

	// 2. ½ºÄ«ÀÌ¹Ú½º ·»´õ¸µ
	descDepthStencil.DepthEnable = TRUE;
	descDepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	// ½ºÄ«ÀÌ¹Ú½º ·»´õ¸µ ½Ã Áß¿ä
	descDepthStencil.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DEPTH_STENCIL_STATETYPE::SKYBOX)].Init(pDevice, &descDepthStencil);

	// 3. °Å¿ï ·»´õ¸µ¿ë
	// ...
	// ...

	// 4. Depth read only
	ZeroMemory(&descDepthStencil, sizeof(descDepthStencil));
	descDepthStencil.DepthEnable = TRUE;
	descDepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	descDepthStencil.DepthFunc = D3D11_COMPARISON_LESS;
	descDepthStencil.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DEPTH_STENCIL_STATETYPE::DEPTH_READ_ONLY)].Init(pDevice, &descDepthStencil);

	// 5. No Depth/Stencil test (Ä«¸Þ¶ó º´ÇÕ µî...)
	ZeroMemory(&descDepthStencil, sizeof(descDepthStencil));
	descDepthStencil.DepthEnable = FALSE;
	descDepthStencil.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DEPTH_STENCIL_STATETYPE::NO_DEPTH_STENCILTEST)].Init(pDevice, &descDepthStencil);
}

void GraphicDeviceImpl::ReleaseDepthStencilStates()
{
	for (size_t i = 0; i < _countof(m_dss); ++i)
		m_dss[i].Release();
}

void GraphicDeviceImpl::CreateBlendStates()
{
	D3D11_BLEND_DESC descBlend;
	ZeroMemory(&descBlend, sizeof(descBlend));
	descBlend.AlphaToCoverageEnable = FALSE;
	descBlend.IndependentBlendEnable = FALSE;
	D3D11_RENDER_TARGET_BLEND_DESC& descRenderTargetBlend = descBlend.RenderTarget[0];

	// 1. Opaque
	descRenderTargetBlend.BlendEnable = FALSE;
	descRenderTargetBlend.SrcBlend = D3D11_BLEND_ONE;
	descRenderTargetBlend.DestBlend = D3D11_BLEND_ZERO;
	descRenderTargetBlend.BlendOp = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.SrcBlendAlpha = D3D11_BLEND_ONE;
	descRenderTargetBlend.DestBlendAlpha = D3D11_BLEND_ZERO;
	descRenderTargetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BLEND_STATETYPE::OPAQUE_)].Init(m_pDevice, &descBlend);

	// 2. AlphaBlend
	descRenderTargetBlend.BlendEnable = TRUE;
	descRenderTargetBlend.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	descRenderTargetBlend.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	
	descRenderTargetBlend.BlendOp = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.SrcBlendAlpha = D3D11_BLEND_ONE;
	descRenderTargetBlend.DestBlendAlpha = D3D11_BLEND_ZERO;
	descRenderTargetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BLEND_STATETYPE::ALPHABLEND)].Init(m_pDevice, &descBlend);

	// 3. No color write
	descRenderTargetBlend.BlendEnable = TRUE;
	descRenderTargetBlend.SrcBlend = D3D11_BLEND_ZERO;
	descRenderTargetBlend.DestBlend = D3D11_BLEND_ONE;
	descRenderTargetBlend.BlendOp = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.SrcBlendAlpha = D3D11_BLEND_ZERO;
	descRenderTargetBlend.DestBlendAlpha = D3D11_BLEND_ONE;
	descRenderTargetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BLEND_STATETYPE::NO_COLOR_WRITE)].Init(m_pDevice, &descBlend);
}

void GraphicDeviceImpl::ReleaseBlendStates()
{
	for (size_t i = 0; i < static_cast<size_t>(BLEND_STATETYPE::COUNT); ++i)
		m_bs[i].Release();
}

UINT GraphicDeviceImpl::GetMSAAMaximumQuality(MSAA_SAMPLE_COUNT sampleCount)
{
	UINT quality = 0;

	// first = Sample count
	// second = Quality level
	for (const auto& info : m_supportedMSAA)
	{
		if (info.first == sampleCount)
		{
			quality = info.second;
			break;
		}
	}

	return quality;
}

void GraphicDeviceImpl::CreateSupportedResolutionInfo()
{
	// DirectX ¹× DXGI ÃÊ±âÈ­
	ComPtr<IDXGIFactory> cpFactory;
	HRESULT hr;

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(cpFactory.GetAddressOf()));
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"CreateSupportedResolutionInfo() > CreateDXGIFactory()", hr);

	// Ã¹ ¹øÂ° ¾î´ðÅÍ¸¦ °¡Á®¿À±â (´ÜÀÏ GPU °¡Á¤)
	ComPtr<IDXGIAdapter> cpAdapter;
	hr = cpFactory->EnumAdapters(0, cpAdapter.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"CreateSupportedResolutionInfo() > IDXGIFactory::EnumAdapters()", hr);

	// Ã¹ ¹øÂ° ¸ð´ÏÅÍ Á¢±Ù
	ComPtr<IDXGIOutput> cpOutput;
	hr = cpAdapter->EnumOutputs(0, cpOutput.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"CreateSupportedResolutionInfo() > IDXGIAdapter::EnumOutputs()", hr);

	// Áö¿øµÇ´Â ÇØ»óµµ ¸ñ·Ï °¡Á®¿À±â
	// ¸ÕÀú °³¼ö È¹µæ
	UINT numModes = 0;
	hr = cpOutput->GetDisplayModeList(BACKBUFFER_FORMAT, 0, &numModes, nullptr);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Failed to load display mode list.", hr);

	m_supportedResolution.resize(numModes);
	hr = cpOutput->GetDisplayModeList(BACKBUFFER_FORMAT, 0, &numModes, m_supportedResolution.data());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Failed to load display mode list.", hr);
}

void GraphicDeviceImpl::CreateSupportedMSAAQualityInfo()
{
	HRESULT hr;

	const MSAA_SAMPLE_COUNT sc[6] = 
	{
		MSAA_SAMPLE_COUNT::OFF, MSAA_SAMPLE_COUNT::X2, MSAA_SAMPLE_COUNT::X4,
		MSAA_SAMPLE_COUNT::X8, MSAA_SAMPLE_COUNT::X16, MSAA_SAMPLE_COUNT::X32
	};

	for (UINT i = 0; i < _countof(sc); ++i)
	{
		UINT quality;
		hr = m_pDevice->CheckMultisampleQualityLevels(BACKBUFFER_FORMAT, static_cast<UINT>(sc[i]), &quality);

		if (SUCCEEDED(hr) && quality != 0)
			m_supportedMSAA.push_back(std::make_pair(sc[i], quality - 1));
	}
}

void GraphicDeviceImpl::InitializeSwapChainAndDepthStencilBufferDesc()
{
	// Initialize SwapChain descriptor.
	m_descSwapChain.BufferDesc.Width = Window.GetWidthInteger();
	m_descSwapChain.BufferDesc.Height = Window.GetHeightInteger();
	m_descSwapChain.BufferDesc.RefreshRate.Numerator = 1;
	m_descSwapChain.BufferDesc.RefreshRate.Denominator = 144;
	m_descSwapChain.BufferDesc.Format = BACKBUFFER_FORMAT;
	m_descSwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_descSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	m_descSwapChain.SampleDesc.Count = 1;
	m_descSwapChain.SampleDesc.Quality = 0;

	m_descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_descSwapChain.BufferCount = 2;
	m_descSwapChain.OutputWindow = Window.GetWindowHandle();
	m_descSwapChain.Windowed = TRUE;		// ÃÖÃÊ¿¡´Â Ã¢ ¸ðµå ½º¿ÒÃ¼ÀÎÀ» »ý¼ºÇÏ´Â °ÍÀ¸·Î (MSDN ³»¿ë Âü°íÇØ¼­..)
	m_descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// Windows 10ºÎÅÍ Áö¿ø!
	m_descSwapChain.Flags = SWAP_CHAIN_FLAG;

	// Depth/Stencil buffer (¹öÆÛ Å©±â´Â ¹Ýµå½Ã ½º¿Ò Ã¼ÀÎÀÇ ¹é ¹öÆÛ¿Í ÀÏÄ¡ÇØ¾ß ÇÔ)
	m_descDepthStencil.Width = m_descSwapChain.BufferDesc.Width;
	m_descDepthStencil.Height = m_descSwapChain.BufferDesc.Height;
	m_descDepthStencil.MipLevels = 1;
	m_descDepthStencil.ArraySize = 1;
	m_descDepthStencil.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Must match with swap chain's MSAA setting!
	m_descDepthStencil.SampleDesc = m_descSwapChain.SampleDesc;

	m_descDepthStencil.Usage = D3D11_USAGE_DEFAULT;
	m_descDepthStencil.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	m_descDepthStencil.CPUAccessFlags = 0;
	m_descDepthStencil.MiscFlags = 0;
}

void GraphicDeviceImpl::CreateCommonVertexBuffers()
{
	// 1. Button VB
	{
		const XMFLOAT2 ltShade = XMFLOAT2(+0.5f, -0.5f);
		const XMFLOAT2 rbShade = XMFLOAT2(ltShade.y, ltShade.x);
		const XMFLOAT2 centerShade = XMFLOAT2(0.0f, 0.0f);
		const float offsetValue = 2.0f;
		VFButton v[30];

		// Top shaded
		v[0].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[0].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[0].m_shade = ltShade;
		v[1].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[1].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[1].m_shade = ltShade;
		v[2].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[2].m_offset = XMFLOAT2(+offsetValue, -offsetValue);
		v[2].m_shade = ltShade;
		v[3].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[3].m_offset = XMFLOAT2(+offsetValue, -offsetValue);
		v[3].m_shade = ltShade;
		v[4].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[4].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[4].m_shade = ltShade;
		v[5].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[5].m_offset = XMFLOAT2(-offsetValue, -offsetValue);
		v[5].m_shade = ltShade;

		// Left shaded
		v[6].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[6].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[6].m_shade = ltShade;
		v[7].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[7].m_offset = XMFLOAT2(+offsetValue, -offsetValue);
		v[7].m_shade = ltShade;
		v[8].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[8].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[8].m_shade = ltShade;
		v[9].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[9].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[9].m_shade = ltShade;
		v[10].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[10].m_offset = XMFLOAT2(+offsetValue, -offsetValue);
		v[10].m_shade = ltShade;
		v[11].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[11].m_offset = XMFLOAT2(+offsetValue, +offsetValue);
		v[11].m_shade = ltShade;

		// Right shaded
		v[12].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[12].m_offset = XMFLOAT2(-offsetValue, +offsetValue);
		v[12].m_shade = rbShade;
		v[13].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[13].m_offset = XMFLOAT2(-offsetValue, -offsetValue);
		v[13].m_shade = rbShade;
		v[14].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[14].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[14].m_shade = rbShade;
		v[15].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[15].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[15].m_shade = rbShade;
		v[16].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[16].m_offset = XMFLOAT2(-offsetValue, -offsetValue);
		v[16].m_shade = rbShade;
		v[17].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[17].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[17].m_shade = rbShade;

		// Bottom shaded
		v[18].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[18].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[18].m_shade = rbShade;
		v[19].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[19].m_offset = XMFLOAT2(+offsetValue, +offsetValue);
		v[19].m_shade = rbShade;
		v[20].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[20].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[20].m_shade = rbShade;
		v[21].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[21].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[21].m_shade = rbShade;
		v[22].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[22].m_offset = XMFLOAT2(+offsetValue, +offsetValue);
		v[22].m_shade = rbShade;
		v[23].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[23].m_offset = XMFLOAT2(-offsetValue, +offsetValue);
		v[23].m_shade = rbShade;

		// Center
		v[24].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[24].m_offset = XMFLOAT2(+offsetValue, +offsetValue);
		v[24].m_shade = centerShade;
		v[25].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[25].m_offset = XMFLOAT2(+offsetValue, -offsetValue);
		v[25].m_shade = centerShade;
		v[26].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[26].m_offset = XMFLOAT2(-offsetValue, +offsetValue);
		v[26].m_shade = centerShade;
		v[27].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[27].m_offset = XMFLOAT2(-offsetValue, +offsetValue);
		v[27].m_shade = centerShade;
		v[28].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[28].m_offset = XMFLOAT2(+offsetValue, -offsetValue);
		v[28].m_shade = centerShade;
		v[29].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[29].m_offset = XMFLOAT2(-offsetValue, -offsetValue);
		v[29].m_shade = centerShade;

		// Create a vertex buffer
		D3D11_BUFFER_DESC descBuffer;
		ZeroMemory(&descBuffer, sizeof(descBuffer));
		descBuffer.ByteWidth = sizeof(v);
		descBuffer.Usage = D3D11_USAGE_IMMUTABLE;
		descBuffer.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		descBuffer.CPUAccessFlags = 0;
		descBuffer.MiscFlags = 0;
		descBuffer.StructureByteStride = sizeof(VFButton);

		D3D11_SUBRESOURCE_DATA sbrcBuffer;
		ZeroMemory(&sbrcBuffer, sizeof(sbrcBuffer));
		sbrcBuffer.pSysMem = v;
		// sbrcBuffer.SysMemPitch = 0;		// unused
		// sbrcBuffer.SysMemSlicePitch = 0;	// unused

		m_vb[static_cast<size_t>(VERTEX_BUFFER_TYPE::BUTTON)].Init(m_pDevice, &descBuffer, &sbrcBuffer);
	}
}

void GraphicDeviceImpl::ReleaseCommonVertexBuffers()
{
	for (size_t i = 0; i < static_cast<size_t>(VERTEX_BUFFER_TYPE::COUNT); ++i)
		m_vb[i].Release();
}

void GraphicDeviceImpl::OnResize()
{
	HRESULT hr;

	/*
		m_cpImmContext->ClearState();

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
	*/

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
	// [Remarks] MSDN ³»¿ë
	// You can't resize a swap chain unless you release all outstanding references to its back buffers. <- Áß¿ä
	// 
	// You must release all of its 'direct' and 'indirect' references on the back buffers in order for ResizeBuffers to succeed.
	// (Á÷Á¢ ÂüÁ¶)
	// 'Direct references' are held by the application after it calls AddRef on a resource.
	// 
	// (°£Á¢ ÂüÁ¶)
	// 'Indirect references' are held by views to a resource, binding a view of the resource to a device context,
	// a command list that used the resource, a command list that used a view to that resource,
	// a command list that executed another command list that used the resource, and so on.

	// ½º¿Ò Ã¼ÀÎ¿¡ °üÇÑ ¸®¼Ò½º¸¦ ¸ðµÎ ÇØÁ¦
	Helper::SafeReleaseCom(m_pSwapChainDSV);
	Helper::SafeReleaseCom(m_pSwapChainRTV);
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬

	// ¸ðµå º¯°æ
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// µð½ºÇÃ·¹ÀÌ°¡ Áö¿øÇÏ´Â ÇØ»óµµ¿©¾ß¸¸ ÇÑ´Ù. (»ç¿ëÀÚ¿¡°Ô ¿­°ÅÇÏ´Â ±â´É ±¸Çö ÇÊ¿ä)
	// MSDN
	// Ã¢ ¸ðµåÇü ½º¿Ò Ã¼ÀÎÀ» »ý¼ºÇÏ°í ÃÖÁ¾ »ç¿ëÀÚ°¡ SetFullscreenState¸¦ ÅëÇØ ½º¿Ò Ã¼ÀÎÀ» ÀüÃ¼ È­¸éÀ¸·Î º¯°æÇÒ ¼ö ÀÖµµ·Ï ÇÏ´Â °ÍÀÌ ÁÁ½À´Ï´Ù.
	// Áï, ½º¿Ò Ã¼ÀÎÀ» ÀüÃ¼ È­¸éÀ¸·Î °­Á¦·Î ¼³Á¤ÇÏ±â À§ÇØ DXGI_SWAP_CHAIN_DESC ÀÇ Windowed ¸â¹ö¸¦ FALSE·Î ¼³Á¤ÇÏÁö ¸¶½Ê½Ã¿À.
	// ±×·¯³ª ½º¿Ò Ã¼ÀÎÀ» ÀüÃ¼ È­¸éÀ¸·Î »ý¼ºÇÏ´Â °æ¿ì ÃÖÁ¾ »ç¿ëÀÚ¿¡°Ô Áö¿øµÇ´Â µð½ºÇÃ·¹ÀÌ ¸ðµå ¸ñ·Ïµµ Á¦°øÇØ¾ß ÇÕ´Ï´Ù.
	// Áö¿øµÇÁö ¾Ê´Â µð½ºÇÃ·¹ÀÌ ¸ðµå·Î »ý¼ºµÈ ½º¿Ò Ã¼ÀÎÀº µð½ºÇÃ·¹ÀÌ°¡ °Ë°Ô º¯ÇÏ°í ÃÖÁ¾ »ç¿ëÀÚ°¡ ¾Æ¹«°Íµµ º¼ ¼ö ¾ø°Ô ¸¸µé ¼ö ÀÖ±â ¶§¹®ÀÔ´Ï´Ù. (Áß¿ä!)
	// ¶ÇÇÑ ÃÖÁ¾ »ç¿ëÀÚ°¡ µð½ºÇÃ·¹ÀÌ ¸ðµå¸¦ º¯°æÇÒ ¼ö ÀÖµµ·Ï Çã¿ëÇÒ ¶§ ½Ã°£ ÃÊ°ú È®ÀÎ È­¸éÀÌ³ª ±âÅ¸ ´ëÃ¼ ¸ÞÄ¿´ÏÁòÀ» °®´Â °ÍÀÌ ÁÁ½À´Ï´Ù.
	const BOOL fullscreen = Window.IsFullscreen() ? TRUE : FALSE;
	hr = m_pSwapChain->SetFullscreenState(fullscreen, nullptr);	// Windowed -> Fullscreen
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::SetFullscreenState()", hr);
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	
	// ½º¿ÒÃ¼ÀÎ Å©±â º¯°æ
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// m_pSwapChain->Resize_target();
	hr = m_pSwapChain->ResizeBuffers(
		0,							// Set this number to zero to preserve the existing number of buffers in the swap chain.
		Window.GetWidthInteger(),
		Window.GetHeightInteger(),
		DXGI_FORMAT_UNKNOWN,		// Set this value to DXGI_FORMAT_UNKNOWN to preserve the existing format of the back buffer.
		SWAP_CHAIN_FLAG
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::ResizeBuffers()", hr);

	// º¯°æµÈ ½º¿Ò Ã¼ÀÎÀÇ Á¤º¸ È¹µæ
	hr = m_pSwapChain->GetDesc(&m_descSwapChain);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::GetDesc()", hr);
	// µª½º½ºÅÙ½Ç ¹öÆÛ Å©±â Á¤º¸µµ ¾÷µ¥ÀÌÆ®
	m_descDepthStencil.Width = m_descSwapChain.BufferDesc.Width;
	m_descDepthStencil.Height = m_descSwapChain.BufferDesc.Height;

	// ½º¿Ò Ã¼ÀÎÀÇ ¹é¹öÆÛ¿¡ ´ëÇÑ ·»´õ Å¸°Ù ºä Àç»ý¼º
	ComPtr<ID3D11Texture2D> cpBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(cpBackBuffer.GetAddressOf()));
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::GetBuffer()", hr);

	assert(m_pSwapChainRTV == nullptr);
	hr = m_pDevice->CreateRenderTargetView(cpBackBuffer.Get(), nullptr, &m_pSwapChainRTV);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateRenderTargetView()", hr);

	// ½º¿Ò Ã¼ÀÎ¿¡ ´ëÇÑ ·»´õ¸µ¿¡ »ç¿ëÇÒ µª½º ½ºÅÙ½Ç ºä Àç»ý¼º
	ComPtr<ID3D11Texture2D> cpDepthStencilBuffer;
	hr = m_pDevice->CreateTexture2D(&m_descDepthStencil, nullptr, cpDepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateTexture2D()", hr);

	assert(m_pSwapChainDSV == nullptr);
	hr = m_pDevice->CreateDepthStencilView(cpDepthStencilBuffer.Get(), nullptr, &m_pSwapChainDSV);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateDepthStencilView()", hr);
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	

	// ½º¿Ò Ã¼ÀÎ Á¤º¸ °»½Å (DXGI´Â ÃÖÁ¾ »ç¿ëÀÚ³ª ½Ã½ºÅÛ ¿äÃ»¿¡ µû¶ó ½º¿Ò Ã¼ÀÎÀÇ Ç¥½Ã »óÅÂ¸¦ º¯°æÇÒ ¼ö ÀÖ½À´Ï´Ù)
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// ½Ã½ºÅÛ ¿äÃ»¿¡ ÀÇÇØ º¯°æµÇ¾úÀ» ¼ö ÀÖÀ¸´Ï ÀÓÀÇ·Î m_descSwapChain¸¦ ¼öµ¿ ¾÷µ¥ÀÌÆ®ÇÏ´Â ¹æ¹ý º¸´Ù´Â ´Ù½Ã GetDesc È£ÃâÇÏ´Â ½ÄÀ¸·Î
	hr = m_pSwapChain->GetDesc(&m_descSwapChain);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::GetDesc()", hr);
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬

	// ÀüÃ¼ ½º¿Ò Ã¼ÀÎ ¿µ¿ªÀ» ³ªÅ¸³»´Â ºäÆ÷Æ® ±¸Á¶Ã¼ ¾÷µ¥ÀÌÆ®
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	m_fullSwapChainViewport.TopLeftX = 0.0f;
	m_fullSwapChainViewport.TopLeftY = 0.0f;
	m_fullSwapChainViewport.Width = static_cast<float>(m_descSwapChain.BufferDesc.Width);
	m_fullSwapChainViewport.Height = static_cast<float>(m_descSwapChain.BufferDesc.Height);
	m_fullSwapChainViewport.MinDepth = 0.0f;
	m_fullSwapChainViewport.MaxDepth = 1.0f;
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
}

HRESULT GraphicDeviceImpl::GetFullscreenState(BOOL* pFullscreen)
{
	assert(pFullscreen != nullptr);

	ComPtr<IDXGIOutput> cpTarget;
	HRESULT hr = m_pSwapChain->GetFullscreenState(pFullscreen, cpTarget.GetAddressOf());
	if (FAILED(hr))
		wprintf(L"GraphicDeviceImpl::GetFullscreenState() > IDXGISwapChain::GetFullscreenState() failed. HRESULT: 0x%x", hr);

	return hr;
}
