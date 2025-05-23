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
	L"VSTransformPNTToHCS.cso"
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
};

// constexpr float allows only one floating point constant to exist in memory, even if it is not encoded in a x86 command.
constexpr DXGI_FORMAT BACKBUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr uint32_t SWAP_CHAIN_FLAG = 0;

GraphicDeviceImpl::GraphicDeviceImpl()
	: m_descAdapter()
	, m_descSwapChain()
	, m_descDepthStencil()
	, m_pDevice(nullptr)
	, m_pImmediateContext(nullptr)
	, m_supportedResolution()
	, m_supportedMSAA()
	, m_pSwapChain(nullptr)
	, m_pSwapChainRTV(nullptr)
	, m_pSwapChainDSV(nullptr)
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

	// 雖錳腎朝 п鼻紫 跡煙 匐儀
	this->CreateSupportedResolutionInfo();

	// ж萄錚橫縑憮 雖錳ж朝 詩じ價Ы葭 蘋遴お 羹觼
	// FEATURE_LEVEL_11_0 devices are required to support 4x MSAA for all render target formats,
	// and 8x MSAA for all render target formats except R32G32B32A32 formats.
	this->CreateSupportedMSAAQualityInfo();

	this->InitializeSwapChainAndDepthStencilBufferDesc();

	// 蝶諜 羹檣 儅撩
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

		// 橫湘攪 薑爾 �僱�
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

		// 蝶諜 羹檣曖 寥幗ぷ縑 渠и 溶渦 顫啃 箔 營儅撩
		ComPtr<ID3D11Texture2D> cpBackBuffer;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(cpBackBuffer.GetAddressOf()));
		if (FAILED(hr))
			break;

		assert(m_pSwapChainRTV == nullptr);
		hr = m_pDevice->CreateRenderTargetView(cpBackBuffer.Get(), nullptr, &m_pSwapChainRTV);
		if (FAILED(hr))
			break;

		// 蝶諜 羹檣縑 渠и 溶渦葭縑 餌辨й 答蝶 蝶蘸褒 箔 營儅撩
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
	// Remark 霤堅
	// 瞪羹�飛橉虞� 蝶諜 羹檣擊 儅撩ж堅 Present蒂 ��轎й 陽 楛顫歜檜 蕨晦纂 彊啪 瞪羹�飛橉� 謙猿й 熱 氈朝 僥薯蒂 п唸ж朝 囀萄.
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

	// DXGI ら曖晦棟 綠�側瘓�
	hr = cpDXGIFactory->MakeWindowAssociation(Window.GetWindowHandle(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGIFactory::MakeWindowAssociation()", hr);

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE RASTERIZER STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	this->CreateRasterizerStates();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE DEPTHSTENCIL STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	this->CreateDepthStencilStates();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE SAMPLER STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	this->CreateSamplerStates();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE SHADERS AND INPUT LAYOUTS 收收收收收收收收收收收收收收收收收收收收收收收收收
	this->CreateShaderAndInputLayout();
}

void GraphicDeviceImpl::Release()
{
	HRESULT hr;

	// 收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE SHADERS AND INPUT LAYOUTS 收收收收收收收收收收收收收收收收收收收收收收收收收
	this->ReleaseShaderAndInputLayout();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE SAMPLER STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	this->ReleaseSamplerStates();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE DEPTHSTENCIL STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	this->ReleaseDepthStencilStates();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE RASTERIZER STATES 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	this->ReleaseRasterizerStates();

	Helper::SafeReleaseCom(m_pSwapChainDSV);
	Helper::SafeReleaseCom(m_pSwapChainRTV);

	// 瞪羹�飛� 賅萄艘棻賊 蝶諜 羹檣 п薯 瞪縑 璽賅萄煎 滲唳п醜撿 觼楚衛 寞雖 陛棟
	// https://learn.microsoft.com/ko-kr/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi#destroying-a-swap-chain
	// '蝶諜 羹檣 餉薯' о跡 霤堅
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

	// 夥檜お 囀萄諦 觼晦 葬欐
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
	// 1. POSITION
	// 唳煎 諫撩
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_SKYBOX_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_SKYBOX_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_P_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_P_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// 曲收 Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION)].Init(
		pDevice,
		VFPosition::GetInputElementDescriptor(),
		VFPosition::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 2. POSITION, COLOR
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PC_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PC_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// 曲收 Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_COLOR)].Init(
		pDevice,
		VFPositionColor::GetInputElementDescriptor(),
		VFPositionColor::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 3. POSITION, NORMAL
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PN_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PN_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// 曲收 Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_NORMAL)].Init(
		pDevice,
		VFPositionNormal::GetInputElementDescriptor(),
		VFPositionNormal::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 4. POSITION, TEXCOORD
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PT_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PT_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// 曲收 Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_TEXCOORD)].Init(
		pDevice,
		VFPositionTexCoord::GetInputElementDescriptor(),
		VFPositionTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 5.TRANSFORM_CAMERA_MERGE_QUAD (No Input Layout required)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_CAMERA_MERGE_QUAD)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_CAMERA_MERGE_QUAD)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 6. POSITION, NORMAL, TEXCOORD
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PNT_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PNT_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// 曲收 Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD)].Init(
		pDevice,
		VFPositionNormalTexCoord::GetInputElementDescriptor(),
		VFPositionNormalTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 7. POSITION, TEXCOORD0, TEXCOORD1
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_TERRAIN_PATCH_CTRL_PT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_TERRAIN_PATCH_CTRL_PT)].Init(pDevice, pByteCode, byteCodeSize);
	// 曲收 Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::TERRAIN_PATCH_CTRL_PT)].Init(
		pDevice,
		VFTerrainPatchControlPoint::GetInputElementDescriptor(),
		VFTerrainPatchControlPoint::GetInputElementCount(),
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

	// 7. PSStandardColoringPNT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PNT_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PNT_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 8. PSCameraMerge (Not implemented)
	/*
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_TEXTURE)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_TEXTURE)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;
	*/

	// 9. PSMSCameraMerge
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_MS_TEXTURE)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_MS_TEXTURE)].Init(pDevice, pByteCode, byteCodeSize);
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
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; // 誰摹⑽ (Bilinear)
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::BILINEAR)].Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // 鳴摹⑽ (Trilinear)
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

	// 1. 橾奩 溶渦葭辨
	descDepthStencil.DepthEnable = TRUE;
	descDepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.DepthFunc = D3D11_COMPARISON_LESS;
	descDepthStencil.StencilEnable = FALSE;	// 橾奩 溶渦葭辨
	m_dss[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::STANDARD)].Init(pDevice, &descDepthStencil);

	// 2. 蝶蘋檜夢蝶 溶渦葭辨
	descDepthStencil.DepthEnable = TRUE;
	descDepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	// 蝶蘋檜夢蝶 溶渦葭 衛 醞蹂
	descDepthStencil.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::SKYBOX)].Init(pDevice, &descDepthStencil);

	// 3. 剪選 溶渦葭辨
	// ...
	// ...

	// 4. 蘋詭塭 煽м辨
	ZeroMemory(&descDepthStencil, sizeof(descDepthStencil));
	descDepthStencil.DepthEnable = FALSE;
	descDepthStencil.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::CAMERA_MERGE)].Init(pDevice, &descDepthStencil);
}

void GraphicDeviceImpl::ReleaseDepthStencilStates()
{
	for (size_t i = 0; i < _countof(m_dss); ++i)
		m_dss[i].Release();
}

void GraphicDeviceImpl::CreateBlendStates()
{
	// D3D11_BLEND_ONE, D3D11_BLEND_ZERO
	D3D11_BLEND srcBlend[static_cast<size_t>(BLEND_STATE_TYPE::COUNT)];
	D3D11_BLEND destBlend[static_cast<size_t>(BLEND_STATE_TYPE::COUNT)];

	// 1. OPAQUE
	srcBlend[static_cast<size_t>(BLEND_STATE_TYPE::OPAQUE_)] = D3D11_BLEND_ONE;
	destBlend[static_cast<size_t>(BLEND_STATE_TYPE::OPAQUE_)] = D3D11_BLEND_ZERO;

	// 2. ALPHABLEND
	srcBlend[static_cast<size_t>(BLEND_STATE_TYPE::ALPHABLEND)] = D3D11_BLEND_ONE;
	destBlend[static_cast<size_t>(BLEND_STATE_TYPE::ALPHABLEND)] = D3D11_BLEND_INV_SRC_ALPHA;

	// 3. ADDITIVE
	srcBlend[static_cast<size_t>(BLEND_STATE_TYPE::ADDITIVE)] = D3D11_BLEND_SRC_ALPHA;
	destBlend[static_cast<size_t>(BLEND_STATE_TYPE::ADDITIVE)] = D3D11_BLEND_ONE;

	// 4. NON_PREMULTIPLIED
	srcBlend[static_cast<size_t>(BLEND_STATE_TYPE::NON_PREMULTIPLIED)] = D3D11_BLEND_SRC_ALPHA;
	destBlend[static_cast<size_t>(BLEND_STATE_TYPE::NON_PREMULTIPLIED)] = D3D11_BLEND_INV_SRC_ALPHA;

	for (size_t i = 0; i < static_cast<size_t>(BLEND_STATE_TYPE::COUNT); ++i)
	{
		D3D11_BLEND_DESC descBlend;
		ZeroMemory(&descBlend, sizeof(descBlend));

		descBlend.RenderTarget[0].BlendEnable = (srcBlend[i] != D3D11_BLEND_ONE) ||
			(destBlend[i] != D3D11_BLEND_ZERO);

		descBlend.RenderTarget[0].SrcBlend = descBlend.RenderTarget[0].SrcBlendAlpha = srcBlend[i];
		descBlend.RenderTarget[0].DestBlend = descBlend.RenderTarget[0].DestBlendAlpha = destBlend[i];
		descBlend.RenderTarget[0].BlendOp = descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		m_bs[i].Init(m_pDevice, &descBlend);
	}
}

void GraphicDeviceImpl::ReleaseBlendStates()
{
	for (size_t i = 0; i < static_cast<size_t>(BLEND_STATE_TYPE::COUNT); ++i)
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
	// DirectX 塽 DXGI 蟾晦��
	ComPtr<IDXGIFactory> cpFactory;
	HRESULT hr;

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(cpFactory.GetAddressOf()));
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"CreateSupportedResolutionInfo() > CreateDXGIFactory()", hr);

	// 羅 廓簞 橫湘攪蒂 陛螳螃晦 (欽橾 GPU 陛薑)
	ComPtr<IDXGIAdapter> cpAdapter;
	hr = cpFactory->EnumAdapters(0, cpAdapter.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"CreateSupportedResolutionInfo() > IDXGIFactory::EnumAdapters()", hr);

	// 羅 廓簞 賅棲攪 蕾斬
	ComPtr<IDXGIOutput> cpOutput;
	hr = cpAdapter->EnumOutputs(0, cpOutput.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"CreateSupportedResolutionInfo() > IDXGIAdapter::EnumOutputs()", hr);

	// 雖錳腎朝 п鼻紫 跡煙 陛螳螃晦
	// 試盪 偃熱 �僱�
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
	m_descSwapChain.BufferDesc.Width = Window.GetWidth();
	m_descSwapChain.BufferDesc.Height = Window.GetHeight();
	m_descSwapChain.BufferDesc.RefreshRate.Numerator = 1;
	m_descSwapChain.BufferDesc.RefreshRate.Denominator = 60;
	m_descSwapChain.BufferDesc.Format = BACKBUFFER_FORMAT;
	m_descSwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_descSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// (纔蝶お) 4X MSAA縑 maximum quailty蒂 堅薑戲煎 餌辨
	// The default sampler mode, with no anti-aliasing, has a count of 1 and a quality level of 0.
	m_descSwapChain.SampleDesc.Count = static_cast<UINT>(MSAA_SAMPLE_COUNT::X4);
	m_descSwapChain.SampleDesc.Quality = this->GetMSAAMaximumQuality(MSAA_SAMPLE_COUNT::X4);	// Use max quality level

	m_descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_descSwapChain.BufferCount = 2;
	m_descSwapChain.OutputWindow = Window.GetWindowHandle();
	m_descSwapChain.Windowed = TRUE;		// 譆蟾縑朝 璽 賅萄 蝶諜羹檣擊 儅撩ж朝 匙戲煎 (MSDN 頂辨 霤堅п憮..)
	m_descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	m_descSwapChain.Flags = SWAP_CHAIN_FLAG;

	// Depth/Stencil buffer (幗ぷ 觼晦朝 奩萄衛 蝶諜 羹檣曖 寥 幗ぷ諦 橾纂п撿 л)
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

	// 蝶諜 羹檣曖 ⑷營 鼻鷓蒂 陛螳螞棻.
	DXGI_SWAP_CHAIN_DESC descSwapChain;
	hr = m_pSwapChain->GetDesc(&descSwapChain);
	if (FAILED(hr))
	{
		wprintf(L"IDXGISwapChain::GetDesc() Exception from HRESULT: 0x%x\n", hr);
		return;
	}

	const bool needResize =
		descSwapChain.BufferDesc.Width != Window.GetWidth() ||
		descSwapChain.BufferDesc.Height != Window.GetHeight();
	const bool isSwapChainFullscreen = descSwapChain.Windowed == FALSE;
	const bool needScreenModeChange = isSwapChainFullscreen != Window.IsFullscreen();

	if (needResize)
	{
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

		// 蝶諜 羹檣縑 婦и 葬模蝶蒂 賅舒 п薯
		Helper::SafeReleaseCom(m_pSwapChainDSV);
		Helper::SafeReleaseCom(m_pSwapChainRTV);

		// m_pSwapChain->ResizeTarget();
		hr = m_pSwapChain->ResizeBuffers(
			0,							// Set this number to zero to preserve the existing number of buffers in the swap chain.
			Window.GetWidth(),
			Window.GetHeight(),
			DXGI_FORMAT_UNKNOWN,		// Set this value to DXGI_FORMAT_UNKNOWN to preserve the existing format of the back buffer.
			SWAP_CHAIN_FLAG
		);
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::ResizeBuffers()", hr);

		// 蝶諜 羹檣曖 寥幗ぷ縑 渠и 溶渦 顫啃 箔 營儅撩
		ComPtr<ID3D11Texture2D> cpBackBuffer;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(cpBackBuffer.GetAddressOf()));
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::GetBuffer()", hr);

		assert(m_pSwapChainRTV == nullptr);
		hr = m_pDevice->CreateRenderTargetView(cpBackBuffer.Get(), nullptr, &m_pSwapChainRTV);
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateRenderTargetView()", hr);

		// 蝶諜 羹檣縑 渠и 溶渦葭縑 餌辨й 答蝶 蝶蘸褒 箔 營儅撩
		ComPtr<ID3D11Texture2D> cpDepthStencilBuffer;
		hr = m_pDevice->CreateTexture2D(&m_descDepthStencil, nullptr, cpDepthStencilBuffer.GetAddressOf());
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateTexture2D()", hr);

		assert(m_pSwapChainDSV == nullptr);
		hr = m_pDevice->CreateDepthStencilView(cpDepthStencilBuffer.Get(), nullptr, &m_pSwapChainDSV);
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateDepthStencilView()", hr);
	}

	if (needScreenModeChange)
	{
		// 蛤蝶Ы溯檜陛 雖錳ж朝 п鼻紫罹撿虜 и棻. (餌辨濠縑啪 翮剪ж朝 晦棟 掘⑷ в蹂)

		// MSDN
		// 璽 賅萄⑽ 蝶諜 羹檣擊 儅撩ж堅 譆謙 餌辨濠陛 SetFullscreenState蒂 鱔п 蝶諜 羹檣擊 瞪羹 �飛橉虞� 滲唳й 熱 氈紫煙 ж朝 匙檜 謠蝗棲棻.
		// 闊, 蝶諜 羹檣擊 瞪羹 �飛橉虞� 鬼薯煎 撲薑ж晦 嬪п DXGI_SWAP_CHAIN_DESC 曖 Windowed 詹幗蒂 FALSE煎 撲薑ж雖 葆褊衛螃.
		// 斜楝釭 蝶諜 羹檣擊 瞪羹 �飛橉虞� 儅撩ж朝 唳辦 譆謙 餌辨濠縑啪 雖錳腎朝 蛤蝶Ы溯檜 賅萄 跡煙紫 薯奢п撿 м棲棻.
		// 雖錳腎雖 彊朝 蛤蝶Ы溯檜 賅萄煎 儅撩脹 蝶諜 羹檣擎 蛤蝶Ы溯檜陛 匐啪 滲ж堅 譆謙 餌辨濠陛 嬴鼠匙紫 獐 熱 橈啪 虜菟 熱 氈晦 陽僥殮棲棻. (醞蹂!)
		// 傳и 譆謙 餌辨濠陛 蛤蝶Ы溯檜 賅萄蒂 滲唳й 熱 氈紫煙 ъ辨й 陽 衛除 蟾婁 �挫� �飛橉抭� 晦顫 渠羹 詭醴棲闈擊 乾朝 匙檜 謠蝗棲棻.

		const BOOL fullscreen = Window.IsFullscreen() ? TRUE : FALSE;
		hr = m_pSwapChain->SetFullscreenState(fullscreen, nullptr);	// Windowed -> Fullscreen
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::SetFullscreenState()", hr);
	}

	// 蝶諜 羹檣 薑爾 偵褐 (DXGI朝 譆謙 餌辨濠釭 衛蝶蠱 蹂羶縑 評塭 蝶諜 羹檣曖 ル衛 鼻鷓蒂 滲唳й 熱 氈蝗棲棻)
	// 衛蝶蠱 蹂羶縑 曖п 滲唳腎歷擊 熱 氈戲棲 歜曖煎 m_descSwapChain 偵褐ж朝 寞徹 爾棻朝 棻衛 GetDesc ��轎ж朝 衝戲煎
	hr = m_pSwapChain->GetDesc(&descSwapChain);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::GetDesc()", hr);
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
