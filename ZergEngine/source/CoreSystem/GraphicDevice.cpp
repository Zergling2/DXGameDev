#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\RenderState.h>

using namespace ze;

GraphicDevice* GraphicDevice::s_pInstance = nullptr;

PCWSTR SHADER_LOAD_FAIL_MSG_FMT = L"Failed to open compiled shader object.\n%s";
PCWSTR SHADER_PATH = L"Engine\\Bin\\Shader\\";
// constexpr float allows only one floating point constant to exist in memory, even if it is not encoded in a x86 command.
constexpr uint32_t SWAP_CHAIN_FLAG = 0;
constexpr DXGI_FORMAT SWAP_CHAIN_FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM;
constexpr DXGI_FORMAT SWAP_CHAIN_RTV_FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

static PCWSTR VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::COUNT)] =
{
	L"VSTransformSkyboxToHCS.cso",
	L"VSTransformTerrainPatchCtrlPt.cso",
	L"VSTransformPToHCS.cso",
	L"VSTransformPCToHCS.cso",
	L"VSTransformPNToHCS.cso",
	L"VSTransformPTToHCS.cso",
	L"VSTransformPNTToHCS.cso",
	L"VSTransformPNTTToHCS.cso",
	L"VSTransformPNTTSkinnedToHCS.cso",
	L"VSTransformButtonToHCS.cso",
	L"VSTransformPTQuadToHCS.cso",
	L"VSTransformScreenRatioQuad.cso"
};
static PCWSTR HULL_SHADER_FILES[static_cast<size_t>(HullShaderType::COUNT)] =
{
	L"HSCalcTerrainTessFactor.cso"
};
static PCWSTR DOMAIN_SHADER_FILES[static_cast<size_t>(DomainShaderType::COUNT)] =
{
	L"DSSampleTerrainHeightMap.cso"
};
static PCWSTR PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::COUNT)] =
{
	L"PSColorSkyboxFragment.cso",
	L"PSColorTerrainFragment.cso",
	L"PSColorPFragment.cso",
	L"PSColorPCFragment.cso",
	L"PSColorPNFragment.cso",
	L"PSColorPTFragmentSingleTexture.cso",
	L"PSColorPTFragmentSingleMSTexture.cso",
	L"PSColorPNTFragment.cso",
	L"PSColorPNTTFragment.cso"
};

GraphicDevice::GraphicDevice()
	: m_descAdapter()
	, m_swapChainDesc()
	, m_swapChainSizeFlt(0.0f, 0.0f)
	, m_swapChainHalfSizeFlt(0.0f, 0.0f)
	, m_entireSwapChainViewport()
	, m_supportedResolution()
	, m_supportedMSAA()
	, m_cpDevice()
	, m_cpImmediateContext()
	, m_cpD2DFactory()
	, m_cpDWriteFactory()
	, m_cpSwapChain()
	, m_cpSwapChainRTV()
	, m_cpSwapChainDSV()
	, m_cpD2DRenderTarget()
	, m_cpD2DSolidColorBrush()
	, m_fontMap()
	, m_vs{}
	, m_hs{}
	, m_ds{}
	, m_ps{}
	, m_il{}
	, m_rs{}
	, m_ss{}
	, m_dss{}
	, m_bs{}
	, m_buttonMeshVB()
	, m_plvMeshVB()
	, m_plvMeshIB()
	, m_slvMeshVB()
	, m_slvMeshIB()
{
}

GraphicDevice::~GraphicDevice()
{
}

void GraphicDevice::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new GraphicDevice();
}

void GraphicDevice::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

bool GraphicDevice::Init(HWND hWnd, uint32_t width, uint32_t height, bool fullscreen)
{
	SyncFileLogger& sfl = Runtime::GetInstance()->GetSyncFileLogger();
	HRESULT hr;

	sfl.Write(L"Initializing GraphicDevice...\n");

	// A. D3D11 디바이스 및 컨텍스트 생성
	// https://learn.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1factory-createdxgisurfacerendertarget(idxgisurface_constd2d1_render_target_properties__id2d1rendertarget)?devlangs=cpp&f1url=%3FappId%3DDev17IDEF1%26l%3DEN-US%26k%3Dk(D2D1%2FID2D1Factory%3A%3ACreateDxgiSurfaceRenderTarget)%3Bk(ID2D1Factory%3A%3ACreateDxgiSurfaceRenderTarget)%3Bk(CreateDxgiSurfaceRenderTarget)%3Bk(DevLang-C%2B%2B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
	// To work with Direct2D, the Direct3D device that provides the IDXGISurface
	// must be created with the D3D11_CREATE_DEVICE_BGRA_SUPPORT flag.
#if defined(DEBUG) || defined(_DEBUG)
	const UINT createDeviceFlag = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#else
	const UINT createDeviceFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif
	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,	// For the Direct2D interoperability with Direc3D 11
	};
	D3D_FEATURE_LEVEL maxSupportedFeatureLevel;

	assert(m_cpDevice == nullptr);
	assert(m_cpImmediateContext == nullptr);
	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlag,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		m_cpDevice.GetAddressOf(),
		&maxSupportedFeatureLevel,
		m_cpImmediateContext.GetAddressOf()
	);
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"D3D11CreateDevice", hr);
		return false;
	}

	if (maxSupportedFeatureLevel < D3D_FEATURE_LEVEL_11_1)
	{
		sfl.Write(L"Device does not support DirectX 11.1 feature level.");
		return false;
	}

	// B. D2D Factory 생성
	assert(m_cpD2DFactory == nullptr);
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), reinterpret_cast<void**>(m_cpD2DFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"D2D1CreateFactory", hr);
		return false;
	}

	// C. DWrite Factory 생성 및 기본 텍스트포맷 생성
	assert(m_cpDWriteFactory == nullptr);
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(m_cpDWriteFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"DWriteCreateFactory", hr);
		return false;
	}

	// 지원되는 해상도 목록 검색
	this->CreateSupportedResolutionInfo();

	// 하드웨어에서 지원하는 멀티샘플링 카운트 체크
	// FEATURE_LEVEL_11_0 devices are required to support 4x MSAA for all render target formats,
	// and 8x MSAA for all render target formats except R32G32B32A32 formats.
	this->CreateSupportedMSAAQualityInfo();

	// 스왑 체인 디스크립터 및 뎁스스텐실 버퍼 디스크립터 초기화
	DXGI_SWAP_CHAIN_DESC descSwapChain;
	ZeroMemory(&descSwapChain, sizeof(descSwapChain));
	descSwapChain.BufferDesc.Width = width;
	descSwapChain.BufferDesc.Height = height;
	descSwapChain.BufferDesc.RefreshRate.Numerator = 1;
	descSwapChain.BufferDesc.RefreshRate.Denominator = 144;
	descSwapChain.BufferDesc.Format = SWAP_CHAIN_FORMAT;
	descSwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	descSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	descSwapChain.SampleDesc.Count = 1;
	descSwapChain.SampleDesc.Quality = 0;
	descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descSwapChain.BufferCount = 2;
	descSwapChain.OutputWindow = hWnd;
	descSwapChain.Windowed = TRUE;		// 최초에는 창 모드 스왑체인을 생성하는 것으로 (MSDN 내용 참고해서..)
	descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// Windows 10부터 지원 (This flag cannot be used with multisampling and partial presentation.)
	descSwapChain.Flags = SWAP_CHAIN_FLAG;

	// 스왑 체인 생성
	// First, Get IDXGIFactory instance
	// DirectX Graphics Infrastructure(DXGI)
	ComPtr<IDXGIDevice> cpDXGIDevice;
	ComPtr<IDXGIAdapter> cpDXGIAdapter;
	ComPtr<IDXGIFactory> cpDXGIFactory;

	// Use the IDXGIFactory instance that was used to create the device! (by COM queries)
	// Device 객체와 연결된 IDXGIFactory 인터페이스 객체를 획득해서 스왑 체인을 생성해야 한다.
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	hr = m_cpDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(cpDXGIDevice.GetAddressOf()));
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID3D11Device::QueryInterface", hr);
		return false;
	}

	hr = cpDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(cpDXGIAdapter.GetAddressOf()));
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"IDXGIDevice::GetParent", hr);
		return false;
	}

	// 어댑터 정보 획득
	hr = cpDXGIAdapter->GetDesc(&m_descAdapter);
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"IDXGIAdapter::GetDesc", hr);
		return false;
	}

	hr = cpDXGIAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(cpDXGIFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"IDXGIAdapter::GetParent", hr);
		return false;
	}

	// 스왑 체인 생성
	assert(m_cpSwapChain == nullptr);
	hr = cpDXGIFactory->CreateSwapChain(m_cpDevice.Get(), &descSwapChain, m_cpSwapChain.GetAddressOf());
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"IDXGIFactory::CreateSwapChain", hr);
		return false;
	}

	// 스왑 체인 정보 저장
	hr = m_cpSwapChain->GetDesc(&m_swapChainDesc);
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"IDXGISwapChain::GetDesc", hr);
		return false;
	}

	m_swapChainSizeFlt = XMFLOAT2(static_cast<FLOAT>(m_swapChainDesc.BufferDesc.Width), static_cast<FLOAT>(m_swapChainDesc.BufferDesc.Height));
	m_swapChainHalfSizeFlt = XMFLOAT2(m_swapChainSizeFlt.x * 0.5f, m_swapChainSizeFlt.y * 0.5f);

	// 전체 스왑 체인 영역을 나타내는 뷰포트 구조체 업데이트
	m_entireSwapChainViewport.TopLeftX = 0.0f;
	m_entireSwapChainViewport.TopLeftY = 0.0f;
	m_entireSwapChainViewport.Width = static_cast<FLOAT>(m_swapChainDesc.BufferDesc.Width);
	m_entireSwapChainViewport.Height = static_cast<FLOAT>(m_swapChainDesc.BufferDesc.Height);
	m_entireSwapChainViewport.MinDepth = 0.0f;
	m_entireSwapChainViewport.MaxDepth = 1.0f;

	// 스왑 체인 정보 기반으로 리소스들 생성
	if (!this->CreateGraphicDeviceResources())
		return false;

	// DXGI 편의기능 비활성화
	hr = cpDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"IDXGIFactory::MakeWindowAssociation", hr);
		return false;
	}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━ CREATE SHADERS AND INPUT LAYOUTS ━━━━━━━━━━━━━━━━━━━━━━━━━
	this->CreateShaderAndInputLayout();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ CREATE RASTERIZER STATES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	this->CreateRasterizerStates();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ CREATE SAMPLER STATES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	this->CreateSamplerStates();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━ CREATE DEPTHSTENCIL STATES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	this->CreateDepthStencilStates();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ CREATE BLEND STATES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	this->CreateBlendStates();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━ CREATE COMMON VERTEX BUFFERS ━━━━━━━━━━━━━━━━━━━━━━━━━━━
	if (!this->CreateCommonGraphicResources())
		return false;

	sfl.Write(L"GraphicDevice init completed.\n");

	return true;
}

void GraphicDevice::UnInit()
{
	HRESULT hr;

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━ RELEASE COMMON VERTEX BUFFERS ━━━━━━━━━━━━━━━━━━━━━━━━━━━
	this->ReleaseCommonGraphicResources();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ RELEASE BLEND STATES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	this->ReleaseBlendStates();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━ RELEASE DEPTHSTENCIL STATES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	this->ReleaseDepthStencilStates();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ RELEASE SAMPLER STATES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	this->ReleaseSamplerStates();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━ RELEASE RASTERIZER STATES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	this->ReleaseRasterizerStates();

	// ━━━━━━━━━━━━━━━━━━━━━━━━ RELEASE SHADERS AND INPUT LAYOUTS ━━━━━━━━━━━━━━━━━━━━━━━━━
	this->ReleaseShaderAndInputLayout();

	// m_supportedMSAA.clear();		// 필수 X

	// 전체화면 모드였다면 스왑 체인 해제 전에 창모드로 변경해줘야 크래시 방지 가능
	// https://learn.microsoft.com/ko-kr/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi#destroying-a-swap-chain
	// '스왑 체인 삭제' 항목 참고
	if (m_cpSwapChain != nullptr)
	{
		BOOL isFullscreen;
		hr = this->GetFullscreenState(&isFullscreen);
		if (SUCCEEDED(hr) && isFullscreen != FALSE)
			m_cpSwapChain->SetFullscreenState(FALSE, nullptr);
	}

	m_fontMap.clear();	// 모든 IDWriteTextFormat COM 객체 해제
	m_cpD2DSolidColorBrush.Reset();
	m_cpD2DRenderTarget.Reset();

	m_cpSwapChainDSV.Reset();
	m_cpSwapChainRTV.Reset();

	m_cpSwapChain.Reset();
	m_cpDWriteFactory.Reset();
	m_cpD2DFactory.Reset();
	m_cpImmediateContext.Reset();
	m_cpDevice.Reset();
}

bool GraphicDevice::LoadShaderByteCode(PCWSTR path, byte** ppByteCode, size_t* pSize)
{
	byte* pByteCode = nullptr;
	FILE* pFile = nullptr;

	errno_t e = _wfopen_s(&pFile, path, L"rb");
	if (e != 0 || pFile == nullptr)
		return false;

	fseek(pFile, 0, SEEK_END);
	const size_t fileSize = static_cast<size_t>(ftell(pFile));
	fseek(pFile, 0, SEEK_SET);

	pByteCode = new byte[fileSize];

	const size_t bytesRead = static_cast<size_t>(fread_s(pByteCode, fileSize, 1, fileSize, pFile));
	// Check error
	assert(bytesRead == fileSize);

	fclose(pFile);

	// 바이트 코드와 크기 리턴
	*ppByteCode = pByteCode;
	*pSize = fileSize;

	return true;
}

void GraphicDevice::CreateShaderAndInputLayout()
{
	ID3D11Device* pDevice = m_cpDevice.Get();

	byte* pByteCode = nullptr;
	size_t byteCodeSize = 0;
	WCHAR targetPath[MAX_PATH];

	// VERTEX SHADERS
	// TransformSkyboxToHCS (No Input Layout required)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformSkyboxToHCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformSkyboxToHCS)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// TransformTerrainPatchCtrlPt (POSITION, TEXCOORD0, TEXCOORD1)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformTerrainPatchCtrlPt)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformTerrainPatchCtrlPt)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::TerrainPatchCtrlPt)].Init(
		pDevice,
		VFTerrainPatchControlPoint::GetInputElementDescriptor(),
		VFTerrainPatchControlPoint::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TransformPToHCS (POSITION)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformPToHCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformPToHCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::Position)].Init(
		pDevice,
		VFPosition::GetInputElementDescriptor(),
		VFPosition::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TransformPCToHCS (POSITION, COLOR0)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformPCToHCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformPCToHCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionColor)].Init(
		pDevice,
		VFPositionColor::GetInputElementDescriptor(),
		VFPositionColor::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TransformPNToHCS (POSITION, NORMAL)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformPNToHCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformPNToHCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionNormal)].Init(
		pDevice,
		VFPositionNormal::GetInputElementDescriptor(),
		VFPositionNormal::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TransformPTToHCS (POSITION, TEXCOORD0)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformPTToHCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformPTToHCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionTexCoord)].Init(
		pDevice,
		VFPositionTexCoord::GetInputElementDescriptor(),
		VFPositionTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TransformPNTToHCS (POSITION, NORMAL, TEXCOORD)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformPNTToHCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformPNTToHCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionNormalTexCoord)].Init(
		pDevice,
		VFPositionNormalTexCoord::GetInputElementDescriptor(),
		VFPositionNormalTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TransformPNTTToHCS (POSITION, NORMAL, TANGENT, TEXCOORD)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformPNTTToHCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformPNTTToHCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionNormalTangentTexCoord)].Init(
		pDevice,
		VFPositionNormalTangentTexCoord::GetInputElementDescriptor(),
		VFPositionNormalTangentTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TransformPNTTSkinnedToHCS (POSITION, NORMAL, TANGENT, TEXCOORD, WEIGHTS, BONEINDICES)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformPNTTSkinnedToHCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformPNTTSkinnedToHCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionNormalTangentTexCoordSkinned)].Init(
		pDevice,
		VFPositionNormalTangentTexCoordSkinned::GetInputElementDescriptor(),
		VFPositionNormalTangentTexCoordSkinned::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TransformButtonToHCS (POSITION, TEXCOORD0, COLOR0)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformButtonToHCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformButtonToHCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::ButtonPt)].Init(
		pDevice,
		VFButton::GetInputElementDescriptor(),
		VFButton::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TransformPTQuadToHCS
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformPTQuadToHCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformPTQuadToHCS)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// TransformScreenRatioQuad (No Input Layout required)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TransformScreenRatioQuad)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TransformScreenRatioQuad)].Init(pDevice, pByteCode, byteCodeSize);
	// No input layout required.
	delete[] pByteCode;





	// HULL SHADERS
	// HS CalcTerrainTessFactor
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), HULL_SHADER_FILES[static_cast<size_t>(HullShaderType::CalcTerrainTessFactor)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_hs[static_cast<size_t>(HullShaderType::CalcTerrainTessFactor)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;





	// DOMAIN SHADERS
	// DS SampleTerrainHeightMap
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), DOMAIN_SHADER_FILES[static_cast<size_t>(DomainShaderType::SampleTerrainHeightMap)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ds[static_cast<size_t>(DomainShaderType::SampleTerrainHeightMap)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;






	// PIXEL SHADERS
	// ColorSkyboxFragment
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorSkyboxFragment)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorSkyboxFragment)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ColorTerrainFragment
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorTerrainFragment)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorTerrainFragment)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ColorPositionFragment
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorPositionFragment)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorPositionFragment)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ColorPositionColorFragment
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorPositionColorFragment)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorPositionColorFragment)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ColorPositionNormalFragment
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorPositionNormalFragment)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorPositionNormalFragment)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ColorPositionTexCoordFragmentWithSingleTexture
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorPositionTexCoordFragmentWithSingleTexture)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorPositionTexCoordFragmentWithSingleTexture)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ColorPositionTexCoordFragmentWithSingleMSTexture
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorPositionTexCoordFragmentWithSingleMSTexture)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorPositionTexCoordFragmentWithSingleMSTexture)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ColorPositionNormalTexCoordFragment
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorPositionNormalTexCoordFragment)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorPositionNormalTexCoordFragment)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ColorPositionNormalTangentTexCoordFragment
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorPositionNormalTangentTexCoordFragment)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorPositionNormalTangentTexCoordFragment)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;
}

void GraphicDevice::ReleaseShaderAndInputLayout()
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

void GraphicDevice::CreateRasterizerStates()
{
	ID3D11Device* pDevice = m_cpDevice.Get();

	D3D11_RASTERIZER_DESC rasterDesc;

	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode = D3D11_CULL_NONE;

	rasterDesc.MultisampleEnable = TRUE;
	m_rs[static_cast<size_t>(RasterizerMode::WireframeMultisample)].Init(
		pDevice, &rasterDesc
	);
	rasterDesc.MultisampleEnable = FALSE;
	m_rs[static_cast<size_t>(RasterizerMode::WireframeNoMultisample)].Init(
		pDevice, &rasterDesc
	);


	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;

	rasterDesc.MultisampleEnable = TRUE;
	m_rs[static_cast<size_t>(RasterizerMode::SolidCullNoneMultisample)].Init(
		pDevice, &rasterDesc
	);
	rasterDesc.MultisampleEnable = FALSE;
	m_rs[static_cast<size_t>(RasterizerMode::SolidCullNoneNoMultisample)].Init(
		pDevice, &rasterDesc
	);


	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_FRONT;

	rasterDesc.MultisampleEnable = TRUE;
	m_rs[static_cast<size_t>(RasterizerMode::SolidCullFrontMultisample)].Init(
		pDevice, &rasterDesc
	);
	rasterDesc.MultisampleEnable = FALSE;
	m_rs[static_cast<size_t>(RasterizerMode::SolidCullFrontNoMultisample)].Init(
		pDevice, &rasterDesc
	);


	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;

	rasterDesc.MultisampleEnable = TRUE;
	m_rs[static_cast<size_t>(RasterizerMode::SolidCullBackMultisample)].Init(
		pDevice, &rasterDesc
	);
	rasterDesc.MultisampleEnable = FALSE;
	m_rs[static_cast<size_t>(RasterizerMode::SolidCullBackNoMultisample)].Init(
		pDevice, &rasterDesc
	);


	/*
	* One of the artifacts with shadow buffer based shadows is shadow acne,
	* or a surface shadowing itself due to minor differences between the depth computation in a shader,
	* and the depth of the same surface in the shadow buffer.
	* 
	* One way to alleviate this is to use DepthBias and SlopeScaledDepthBias when rendering a shadow buffer.
	* The idea is to push surfaces out enough while rendering a shadow buffer so that the 
	* comparison result (between the shadow buffer z and the shader z) is consistent across the surface, and avoid local self-shadowing.
	*/
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 100000;	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;	(UNORM 깊이 버퍼일 경우 계산식)
	/*
	* if(DepthBiasClamp > 0)
    *	Bias = min(DepthBiasClamp, Bias)
	* else if(DepthBiasClamp < 0)
    *	Bias = max(DepthBiasClamp, Bias)
	*/
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.SlopeScaledDepthBias = 1.0f;
	// rasterDesc.MultisampleEnable = FALSE;
	// rasterDesc.AntialiasedLineEnable = FALSE;
	m_rs[static_cast<size_t>(RasterizerMode::ShadowMap)].Init(
		pDevice, &rasterDesc
	);
}

void GraphicDevice::ReleaseRasterizerStates()
{
	for (size_t i = 0; i < static_cast<size_t>(RasterizerMode::COUNT); ++i)
		m_rs[i].Release();
}

void GraphicDevice::CreateSamplerStates()
{
	ID3D11Device* pDevice = m_cpDevice.Get();

	D3D11_SAMPLER_DESC samplerDesc;

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;		// 포인트 샘플링
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TextureFilteringMode::Point)].Init(pDevice, &samplerDesc);

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; // 쌍선형 (Bilinear)
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TextureFilteringMode::Bilinear)].Init(pDevice, &samplerDesc);

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // 삼선형 (Trilinear)
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TextureFilteringMode::Trilinear)].Init(pDevice, &samplerDesc);

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	samplerDesc.MaxAnisotropy = 2;
	m_ss[static_cast<size_t>(TextureFilteringMode::Anisotropic2x)].Init(pDevice, &samplerDesc);

	samplerDesc.MaxAnisotropy = 4;
	m_ss[static_cast<size_t>(TextureFilteringMode::Anisotropic4x)].Init(pDevice, &samplerDesc);

	samplerDesc.MaxAnisotropy = 8;
	m_ss[static_cast<size_t>(TextureFilteringMode::Anisotropic8x)].Init(pDevice, &samplerDesc);

	samplerDesc.MaxAnisotropy = 16;
	m_ss[static_cast<size_t>(TextureFilteringMode::Anisotropic16x)].Init(pDevice, &samplerDesc);


	// Shadow map sampler
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TextureFilteringMode::ShadowMapSampling)].Init(pDevice, &samplerDesc);
}

void GraphicDevice::ReleaseSamplerStates()
{
	for (size_t i = 0; i < _countof(m_ss); ++i)
		m_ss[i].Release();
}

void GraphicDevice::CreateDepthStencilStates()
{
	ID3D11Device* pDevice = m_cpDevice.Get();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	// Default
	depthStencilDesc.DepthEnable = TRUE;							// 뎁스 테스트 활성화
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 깊이 값 기록 허용
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;				// 더 가까운 픽셀만 그리기
	depthStencilDesc.StencilEnable = FALSE;							// 스텐실 비활성화
	m_dss[static_cast<size_t>(DepthStencilStateType::Default)].Init(pDevice, &depthStencilDesc);

	// 스카이박스 렌더링
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	// 스카이박스 렌더링 시 중요
	depthStencilDesc.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DepthStencilStateType::Skybox)].Init(pDevice, &depthStencilDesc);

	// Depth read only (D3D11_COMPARISON_LESS)
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DepthStencilStateType::DepthReadOnlyLess)].Init(pDevice, &depthStencilDesc);

	// Depth read only (D3D11_COMPARISON_LESS_EQUAL)
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DepthStencilStateType::DepthReadOnlyLessEqual)].Init(pDevice, &depthStencilDesc);

	// 5. No Depth/Stencil test (카메라 병합 등...)
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DepthStencilStateType::NoDepthStencilTest)].Init(pDevice, &depthStencilDesc);
}

void GraphicDevice::ReleaseDepthStencilStates()
{
	for (size_t i = 0; i < _countof(m_dss); ++i)
		m_dss[i].Release();
}

void GraphicDevice::CreateBlendStates()
{
	ID3D11Device* pDevice = m_cpDevice.Get();

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	D3D11_RENDER_TARGET_BLEND_DESC& descRenderTargetBlend = blendDesc.RenderTarget[0];

	// 1. Opaque
	descRenderTargetBlend.BlendEnable = FALSE;
	descRenderTargetBlend.SrcBlend = D3D11_BLEND_ONE;
	descRenderTargetBlend.DestBlend = D3D11_BLEND_ZERO;
	descRenderTargetBlend.BlendOp = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.SrcBlendAlpha = D3D11_BLEND_ONE;
	descRenderTargetBlend.DestBlendAlpha = D3D11_BLEND_ZERO;
	descRenderTargetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BlendStateType::Opaque)].Init(pDevice, &blendDesc);

	// 2. AlphaBlend
	descRenderTargetBlend.BlendEnable = TRUE;
	descRenderTargetBlend.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	descRenderTargetBlend.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	
	descRenderTargetBlend.BlendOp = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.SrcBlendAlpha = D3D11_BLEND_ONE;
	descRenderTargetBlend.DestBlendAlpha = D3D11_BLEND_ZERO;
	descRenderTargetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BlendStateType::AlphaBlend)].Init(pDevice, &blendDesc);

	// 3. No color write
	descRenderTargetBlend.BlendEnable = TRUE;
	descRenderTargetBlend.SrcBlend = D3D11_BLEND_ZERO;
	descRenderTargetBlend.DestBlend = D3D11_BLEND_ONE;
	descRenderTargetBlend.BlendOp = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.SrcBlendAlpha = D3D11_BLEND_ZERO;
	descRenderTargetBlend.DestBlendAlpha = D3D11_BLEND_ONE;
	descRenderTargetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BlendStateType::NoColorWrite)].Init(pDevice, &blendDesc);
}

void GraphicDevice::ReleaseBlendStates()
{
	for (size_t i = 0; i < static_cast<size_t>(BlendStateType::COUNT); ++i)
		m_bs[i].Release();
}

bool GraphicDevice::CreateCommonGraphicResources()
{
	ID3D11Device* pDevice = this->GetDeviceComInterface();

	// 1. Button VB
	{
		const XMFLOAT2 ltShade = XMFLOAT2(+0.5f, -0.5f);			// 음영 처리값
		const XMFLOAT2 rbShade = XMFLOAT2(ltShade.y, ltShade.x);	// 음영 처리값
		const XMFLOAT2 centerShade = XMFLOAT2(0.0f, 0.0f);
		const float offsetValue = 2.0f;		// 버튼 입체감 모서리 오프셋
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
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = sizeof(v);
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = sizeof(VFButton);

		D3D11_SUBRESOURCE_DATA subrcData;
		ZeroMemory(&subrcData, sizeof(subrcData));
		subrcData.pSysMem = v;
		// subrcData.SysMemPitch = 0;		// unused
		// subrcData.SysMemSlicePitch = 0;	// unused

		if (!m_buttonMeshVB.Init(pDevice, &bufferDesc, &subrcData))
			return false;
	}

	// 2. Light volume meshes
	{
		FILE* pFile = nullptr;
		size_t numItemsRead;
		int32_t vertexCount;
		int32_t indexCount;

		// ############################################################################
		// Load point light volume mesh
		errno_t e = _wfopen_s(&pFile, L"Engine\\Bin\\Data\\plvmesh", L"rb");
		if (e != 0 || pFile == nullptr)
			Debug::ForceCrashWithMessageBox(L"Error", L"plvmesh file not found");

		// 정점 개수 읽기
		numItemsRead = static_cast<size_t>(fread_s(&vertexCount, sizeof(vertexCount), sizeof(vertexCount), 1, pFile));
		assert(numItemsRead == 1);	// 4바이트 읽혔는지 체크

		// 정점 바이너리 데이터 읽기
		std::vector<VFPosition> vertices(vertexCount);
		numItemsRead = static_cast<size_t>(fread_s(vertices.data(), sizeof(VFPosition) * vertices.size(), sizeof(VFPosition), vertexCount, pFile));
		assert(numItemsRead == vertexCount);

		// 인덱스 개수 읽기
		numItemsRead = static_cast<size_t>(fread_s(&indexCount, sizeof(indexCount), sizeof(indexCount), 1, pFile));
		assert(numItemsRead == 1);	// 4바이트 읽혔는지 체크

		// 인덱스 바이너리 데이터 읽기
		std::vector<uint32_t> indices(indexCount);
		numItemsRead = static_cast<size_t>(fread_s(indices.data(), sizeof(uint32_t) * indices.size(), sizeof(uint32_t), indexCount, pFile));
		assert(numItemsRead == indexCount);

		fclose(pFile);

		// Create a vertex buffer
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(VFPosition) * vertices.size());
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		// bufferDesc.CPUAccessFlags = 0;
		// bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = sizeof(VFPosition);

		D3D11_SUBRESOURCE_DATA subrcData;
		ZeroMemory(&subrcData, sizeof(subrcData));
		subrcData.pSysMem = vertices.data();
		// subrcData.SysMemPitch = 0;		// unused
		// subrcData.SysMemSlicePitch = 0;	// unused

		if (!m_plvMeshVB.Init(pDevice, &bufferDesc, &subrcData))
			return false;

		// Create an index buffer
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indices.size());
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		// bufferDesc.CPUAccessFlags = 0;
		// bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = sizeof(VFPosition);

		ZeroMemory(&subrcData, sizeof(subrcData));
		subrcData.pSysMem = indices.data();
		// subrcData.SysMemPitch = 0;		// unused
		// subrcData.SysMemSlicePitch = 0;	// unused
		if (!m_plvMeshIB.Init(pDevice, &bufferDesc, &subrcData))
			return false;


		// ############################################################################
		// Load spot light volume mesh
		e = _wfopen_s(&pFile, L"Engine\\Bin\\Data\\slvmesh", L"rb");
		if (e != 0 || pFile == nullptr)
			Debug::ForceCrashWithMessageBox(L"Error", L"slvmesh file not found");
		
		// 정점 개수 읽기
		numItemsRead = static_cast<size_t>(fread_s(&vertexCount, sizeof(vertexCount), sizeof(vertexCount), 1, pFile));
		assert(numItemsRead == 1);	// 4바이트 읽혔는지 체크

		// 정점 바이너리 데이터 읽기
		vertices.resize(vertexCount);
		numItemsRead = static_cast<size_t>(fread_s(vertices.data(), sizeof(VFPosition) * vertices.size(), sizeof(VFPosition), vertexCount, pFile));
		assert(numItemsRead == vertexCount);

		// 인덱스 개수 읽기
		numItemsRead = static_cast<size_t>(fread_s(&indexCount, sizeof(indexCount), sizeof(indexCount), 1, pFile));
		assert(numItemsRead == 1);	// 4바이트 읽혔는지 체크

		// 인덱스 바이너리 데이터 읽기
		indices.resize(indexCount);
		numItemsRead = static_cast<size_t>(fread_s(indices.data(), sizeof(uint32_t) * indices.size(), sizeof(uint32_t), indexCount, pFile));
		assert(numItemsRead == indexCount);

		fclose(pFile);

		// Create a vertex buffer
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(VFPosition) * vertices.size());
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		// bufferDesc.CPUAccessFlags = 0;
		// bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = sizeof(VFPosition);

		ZeroMemory(&subrcData, sizeof(subrcData));
		subrcData.pSysMem = vertices.data();
		// subrcData.SysMemPitch = 0;		// unused
		// subrcData.SysMemSlicePitch = 0;	// unused

		if (!m_slvMeshVB.Init(pDevice, &bufferDesc, &subrcData))
			return false;

		// Create an index buffer
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indices.size());
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		// bufferDesc.CPUAccessFlags = 0;
		// bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = sizeof(VFPosition);

		ZeroMemory(&subrcData, sizeof(subrcData));
		subrcData.pSysMem = indices.data();
		// subrcData.SysMemPitch = 0;		// unused
		// subrcData.SysMemSlicePitch = 0;	// unused
		if (!m_slvMeshIB.Init(pDevice, &bufferDesc, &subrcData))
			return false;
	}

	return true;
}

void GraphicDevice::ReleaseCommonGraphicResources()
{
	m_buttonMeshVB.Release();
	m_plvMeshVB.Release();
	m_plvMeshIB.Release();
	m_slvMeshVB.Release();
	m_slvMeshIB.Release();
}

std::shared_ptr<DWriteTextFormatWrapper> GraphicDevice::GetDWriteTextFormatWrapper(const TextFormat& tf)
{
	HRESULT hr;
	std::shared_ptr<DWriteTextFormatWrapper> spReturn;
	std::unordered_map<TextFormat, std::weak_ptr<DWriteTextFormatWrapper>, TextFormatHasher>::iterator iter = m_fontMap.find(tf);
	bool createFont;
	bool reuseNode = false;

	if (iter != m_fontMap.end())	// 노드가 존재하는 경우
	{
		std::weak_ptr<DWriteTextFormatWrapper> wpTextFormat = iter->second;
		if (!wpTextFormat.expired())	// 포인터도 살아있는 경우
		{
			createFont = false;
			spReturn = wpTextFormat.lock();
		}
		else	// 노드는 존재하지만 포인터는 만료된 경우
		{
			// 해시맵 노드는 재사용 가능하므로 second의 weak_ptr만 다시 살아있는 포인터를 대입해준다.
			createFont = true;
			reuseNode = true;
		}
	}
	else
	{
		createFont = true;
		reuseNode = false;
	}

	if (createFont)
	{
		// 폰트 및 해시맵 노드 새로 생성
		// 새로 생성
		IDWriteTextFormat* pTextFormat = nullptr;
		hr = m_cpDWriteFactory->CreateTextFormat(
			tf.GetFontFamilyName().c_str(),
			nullptr,
			tf.GetWeight(),
			tf.GetStyle(),
			tf.GetStretch(),
			static_cast<FLOAT>(tf.GetSize()),
			L"",
			&pTextFormat
		);
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTMessageBox(L"IDWriteFactory::CreateTextFormat()", hr);


		// A. 
		// spReturn = std::make_shared<DWriteTextFormatWrapper>(pTextFormat);	// weak_ptr만 남았을 때에도 메모리가 해제될 수 있도록
		// 제어 블록과 별도로 할당하기 위해 std::make_shared를 사용하지 않는다. 대신 아래의 B 방법을 사용해 객체와 제어 블록을 별도로 유지.

		// B.
		DWriteTextFormatWrapper* pWrapper;
		try
		{
			pWrapper = new DWriteTextFormatWrapper(pTextFormat);	// 메모리 할당에 대한 try catch, DWriteTextFormatWrapper 생성자는 noexcept이므로 예외 걱정 X
		}
		catch (const std::bad_alloc& e)	// 발생할 수 있는 예외는 메모리 할당 실패로 인한 std::bad_alloc 뿐이다.
		{
			UNREFERENCED_PARAMETER(e);
			pWrapper = nullptr;	// 오류 조기 감지
		}
		spReturn = std::shared_ptr<DWriteTextFormatWrapper>(pWrapper);

		if (reuseNode)
		{
			iter->second = spReturn;
		}
	}

	return spReturn;
}

void GraphicDevice::CreateSupportedResolutionInfo()
{
	// DirectX 및 DXGI 초기화
	ComPtr<IDXGIFactory> cpFactory;
	HRESULT hr;

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(cpFactory.GetAddressOf()));
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"CreateSupportedResolutionInfo() > CreateDXGIFactory()", hr);

	// 첫 번째 어댑터를 가져오기 (단일 GPU 가정)
	ComPtr<IDXGIAdapter> cpAdapter;
	hr = cpFactory->EnumAdapters(0, cpAdapter.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"CreateSupportedResolutionInfo() > IDXGIFactory::EnumAdapters()", hr);

	// 첫 번째 모니터 접근
	ComPtr<IDXGIOutput> cpOutput;
	hr = cpAdapter->EnumOutputs(0, cpOutput.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"CreateSupportedResolutionInfo() > IDXGIAdapter::EnumOutputs()", hr);

	// 지원되는 해상도 목록 가져오기
	// 먼저 개수 획득
	UINT numModes = 0;
	hr = cpOutput->GetDisplayModeList(SWAP_CHAIN_FORMAT, 0, &numModes, nullptr);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"Failed to load display mode list.", hr);

	m_supportedResolution.resize(numModes);
	hr = cpOutput->GetDisplayModeList(SWAP_CHAIN_FORMAT, 0, &numModes, m_supportedResolution.data());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"Failed to load display mode list.", hr);
}

void GraphicDevice::CreateSupportedMSAAQualityInfo()
{
	HRESULT hr;

	const MSAAMode sc[] = { MSAAMode::Off, MSAAMode::x2, MSAAMode::x4, MSAAMode::x8 };

	for (UINT i = 0; i < _countof(sc); ++i)
	{
		UINT quality;
		hr = m_cpDevice->CheckMultisampleQualityLevels(SWAP_CHAIN_FORMAT, static_cast<UINT>(sc[i]), &quality);

		if (SUCCEEDED(hr) && quality != 0)
			m_supportedMSAA.push_back(std::make_pair(sc[i], quality - 1));
	}
}

bool GraphicDevice::CreateGraphicDeviceResources()
{
	SyncFileLogger& sfl = Runtime::GetInstance()->GetSyncFileLogger();
	HRESULT hr;

	sfl.Write(L"Creating graphic device resources...\n");
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// D3D 리소스 재생성
	// 스왑 체인의 백버퍼에 대한 렌더 타겟 뷰 재생성
	ComPtr<ID3D11Texture2D> cpBackBuffer;
	hr = m_cpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(cpBackBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"IDXGISwapChain::GetBuffer", hr);
		return false;
	}

	assert(m_cpSwapChainRTV == nullptr);
	D3D11_RENDER_TARGET_VIEW_DESC swapChainRTVDesc;
	ZeroMemory(&swapChainRTVDesc, sizeof(swapChainRTVDesc));
	swapChainRTVDesc.Format = SWAP_CHAIN_RTV_FORMAT;		// sRGB
	swapChainRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	swapChainRTVDesc.Texture2D.MipSlice = 0;

	hr = m_cpDevice->CreateRenderTargetView(cpBackBuffer.Get(), &swapChainRTVDesc, m_cpSwapChainRTV.GetAddressOf());
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID3D11Device::CreateRenderTargetView", hr);
		return false;
	}

	// 스왑 체인에 대한 렌더링에 사용할 뎁스 스텐실 버퍼 및 뷰 재생성
	// 뎁스스텐실 버퍼 재생성을 위해 디스크립터 세팅
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(depthStencilBufferDesc));
	depthStencilBufferDesc.Width = m_swapChainDesc.BufferDesc.Width;
	depthStencilBufferDesc.Height = m_swapChainDesc.BufferDesc.Height;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D16_UNORM;
	// Must match with swap chain's MSAA setting!
	depthStencilBufferDesc.SampleDesc = m_swapChainDesc.SampleDesc;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> cpDepthStencilBuffer;	// DSV가 뎁스 스텐실 버퍼를 간접 참조하므로 지역 ComPtr로 두어도 됨
	hr = m_cpDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, cpDepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID3D11Device::CreateTexture2D", hr);
		return false;
	}

	assert(m_cpSwapChainDSV == nullptr);
	hr = m_cpDevice->CreateDepthStencilView(cpDepthStencilBuffer.Get(), nullptr, m_cpSwapChainDSV.GetAddressOf());
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID3D11Device::CreateDepthStencilView", hr);
		return false;
	}
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// D2D 리소스 재생성
	// 스왑 체인의 백 버퍼에 대한 Direct2D용 DXGI Surface 렌더 타겟 생성
	ComPtr<IDXGISurface> cpBackBufferSurface;
	hr = m_cpSwapChain->GetBuffer(0, IID_PPV_ARGS(cpBackBufferSurface.GetAddressOf()));
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"IDXGISwapChain::GetBuffer", hr);
		return false;
	}

	assert(m_cpD2DRenderTarget == nullptr);
	const D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);
	// Create a Direct2D render target that can draw into the surface in the swap chain
	hr = m_cpD2DFactory->CreateDxgiSurfaceRenderTarget(
		cpBackBufferSurface.Get(),
		&props,
		m_cpD2DRenderTarget.GetAddressOf()
	);
	
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID2D1Factory::CreateDxgiSurfaceRenderTarget", hr);
		return false;
	}

	assert(m_cpD2DSolidColorBrush == nullptr);
	hr = m_cpD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), m_cpD2DSolidColorBrush.GetAddressOf());
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID2D1RenderTarget::CreateSolidColorBrush", hr);
		return false;
	}

	sfl.Write(L"Completed creating graphic device resources.\n");

	return true;
}

void GraphicDevice::ReleaseGraphicDeviceResources()
{
	// https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
	// [Remarks] MSDN 내용
	// You can't resize a swap chain unless you release all outstanding references to its back buffers. <- 중요
	// 
	// You must release all of its 'direct' and 'indirect' references on the back buffers in order for ResizeBuffers to succeed.
	// (직접 참조)
	// 'Direct references' are held by the application after it calls AddRef on a resource.
	// 
	// (간접 참조)
	// 'Indirect references' are held by views to a resource, binding a view of the resource to a device context,
	// a command list that used the resource, a command list that used a view to that resource,
	// a command list that executed another command list that used the resource, and so on.

	// 스왑 체인에 관한 리소스를 모두 해제
	// D2D 리소스들
	m_cpD2DSolidColorBrush.Reset();
	m_cpD2DRenderTarget.Reset();
	// D3D 리소스들
	m_cpSwapChainDSV.Reset();
	m_cpSwapChainRTV.Reset();
}

UINT GraphicDevice::GetMSAAMaximumQuality(MSAAMode sampleCount)
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

bool GraphicDevice::ResizeBuffer(uint32_t width, uint32_t height)
{
	SyncFileLogger& sfl = Runtime::GetInstance()->GetSyncFileLogger();
	HRESULT hr;

	this->ReleaseGraphicDeviceResources();
	/*
	* m_cpImmContext->ClearState();
	* 
	* This method resets any device context to the default settings.
	* This sets
	* 1. all input / output resource slots
	* 2. shaders
	* 3. input layouts
	* 4. predications
	* 5. scissor rectangles
	* 6. depth - stencil state
	* 7. rasterizer state
	* 8. blend state
	* 9. sampler state
	* 10. viewports
	* to NULL.
	* The primitive topology is set to UNDEFINED.
	* For a scenario where you would like to clear a list of commands recorded so far,
	* call ID3D11DeviceContext::FinishCommandList and throw away the resulting ID3D11CommandList.
	*/

	// 모드 변경
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 디스플레이가 지원하는 해상도여야만 한다. (사용자에게 열거하는 기능 구현 필요)
	// MSDN
	// 창 모드형 스왑 체인을 생성하고 최종 사용자가 SetFullscreenState를 통해 스왑 체인을 전체 화면으로 변경할 수 있도록 하는 것이 좋습니다.
	// 즉, 스왑 체인을 전체 화면으로 강제로 설정하기 위해 DXGI_SWAP_CHAIN_DESC 의 Windowed 멤버를 FALSE로 설정하지 마십시오.
	// 그러나 스왑 체인을 전체 화면으로 생성하는 경우 최종 사용자에게 지원되는 디스플레이 모드 목록도 제공해야 합니다.
	// 지원되지 않는 디스플레이 모드로 생성된 스왑 체인은 디스플레이가 검게 변하고 최종 사용자가 아무것도 볼 수 없게 만들 수 있기 때문입니다.
	// 또한 최종 사용자가 디스플레이 모드를 변경할 수 있도록 허용할 때 시간 초과 확인 화면이나 기타 대체 메커니즘을 갖는 것이 좋습니다.
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

	// 스왑체인 크기 변경
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// IDXGISwapChain::ResizeTarget()
	hr = m_cpSwapChain->ResizeBuffers(
		0,							// Set this number to zero to preserve the existing number of buffers in the swap chain.
		width,
		height,
		DXGI_FORMAT_UNKNOWN,		// Set this value to DXGI_FORMAT_UNKNOWN to preserve the existing format of the back buffer.
		SWAP_CHAIN_FLAG
	);
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"IDXGISwapChain::ResizeBuffers", hr);
		return false;
	}

	// 스왑 체인 정보 저장
	hr = m_cpSwapChain->GetDesc(&m_swapChainDesc);
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"IDXGISwapChain::GetDesc", hr);
		return false;
	}

	m_swapChainSizeFlt = XMFLOAT2(static_cast<FLOAT>(m_swapChainDesc.BufferDesc.Width), static_cast<FLOAT>(m_swapChainDesc.BufferDesc.Height));
	m_swapChainHalfSizeFlt = XMFLOAT2(m_swapChainSizeFlt.x * 0.5f, m_swapChainSizeFlt.y * 0.5f);

	// 전체 스왑 체인 영역을 나타내는 뷰포트 구조체 업데이트
	m_entireSwapChainViewport.TopLeftX = 0.0f;
	m_entireSwapChainViewport.TopLeftY = 0.0f;
	m_entireSwapChainViewport.Width = static_cast<FLOAT>(m_swapChainDesc.BufferDesc.Width);
	m_entireSwapChainViewport.Height = static_cast<FLOAT>(m_swapChainDesc.BufferDesc.Height);
	m_entireSwapChainViewport.MinDepth = 0.0f;
	m_entireSwapChainViewport.MaxDepth = 1.0f;


	// 변경된 스왑 체인 정보 기반으로 리소스들 재생성
	if (!this->CreateGraphicDeviceResources())
		return false;

	return true;
}

HRESULT GraphicDevice::SetFullscreenState(BOOL fullscreen)
{
	// If you pass FALSE to Fullscreen, then you must set this parameter to NULL
	HRESULT hr = m_cpSwapChain->SetFullscreenState(fullscreen, nullptr);

	return hr;
}

HRESULT GraphicDevice::GetFullscreenState(BOOL* pFullscreen)
{
	assert(pFullscreen != nullptr);

	HRESULT hr = m_cpSwapChain->GetFullscreenState(pFullscreen, nullptr);

	return hr;
}
