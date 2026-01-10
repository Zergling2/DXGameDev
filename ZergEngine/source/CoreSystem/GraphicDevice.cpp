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
	L"VSToHcsDebugLines.cso",
	L"VSToHcsP.cso",
	L"VSToHcsPC.cso",
	L"VSToHcsPN.cso",
	L"VSToHcsPT.cso",
	L"VSToHcsPNT.cso",
	L"VSToHcsPNTT.cso",
	L"VSToHcsPNTTSkinned.cso",
	L"VSTerrainPatchCtrlPt.cso",
	L"VSToHcsSkybox.cso",
	L"VSToHcsPNTTQuadForBillboard.cso",
	L"VSToHcsPNTTQuadForImage.cso",
	L"VSToHcsPNTTQuadForShadedEdgeQuad.cso",
	L"VSToHcsPNTTQuadForShadedEdgeCircle.cso",
	L"VSToHcsCheckbox.cso",
	L"VSToHcsScreenRatioQuad.cso"
	// L"VSToHcsShaded2DQuad.cso",
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
	L"PSUnlitP.cso",
	L"PSUnlitPC.cso",
	L"PSUnlitPNNoMtl.cso",
	L"PSLitPN.cso",
	L"PSUnlitPT1Tex.cso",
	L"PSUnlitPT1MSTex.cso",
	// L"PSLitPNT.cso",		// deprecated
	L"PSUnlitPNTTNoMtl.cso",
	L"PSLitPNTT.cso",
	L"PSLitPNTTDiffMapping.cso",
	L"PSLitPNTTSpecMapping.cso",
	L"PSLitPNTTNormMapping.cso",
	L"PSLitPNTTDiffSpecMapping.cso",
	L"PSLitPNTTDiffNormMapping.cso",
	L"PSLitPNTTSpecNormMapping.cso",
	L"PSLitPNTTDiffSpecNormMapping.cso",
	L"PSSkyboxFragment.cso",
	L"PSLitTerrainFragment.cso",
	L"PSColorShadedEdgeQuad.cso",
	L"PSColorShadedEdgeCircle.cso"
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
	, m_cpRTVSwapChain()
	, m_cpDSVSwapChain()
	, m_cpD2DRTBackBuffer()
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
	// , m_vbShaded2DQuad()
	, m_vbCheckbox()
	, m_vbPNTTQuad()
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
	// First, Get IDXGIFactory instance [DirectX Graphics Infrastructure(DXGI)]
	// Use the IDXGIFactory instance that was used to create the device! (by COM queries)
	// Device 객체와 연결된 IDXGIFactory 인터페이스 객체를 획득해서 스왑 체인을 생성해야 한다.
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	ComPtr<IDXGIDevice> cpDXGIDevice;
	hr = m_cpDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(cpDXGIDevice.GetAddressOf()));
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID3D11Device::QueryInterface", hr);
		return false;
	}

	ComPtr<IDXGIAdapter> cpDXGIAdapter;
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

	ComPtr<IDXGIFactory> cpDXGIFactory;
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
	m_cpD2DRTBackBuffer.Reset();

	m_cpDSVSwapChain.Reset();
	m_cpRTVSwapChain.Reset();

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
	// ToHcsDebugLines
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsDebugLines)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsDebugLines)].Init(pDevice, pByteCode, byteCodeSize);
	// VFPositionColor Input Layout 사용
	delete[] pByteCode;

	
	// ToHcsP
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsP)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsP)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::Position)].Init(
		pDevice,
		VFPosition::GetInputElementDescriptor(),
		VFPosition::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// ToHcsPC
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsPC)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsPC)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionColor)].Init(
		pDevice,
		VFPositionColor::GetInputElementDescriptor(),
		VFPositionColor::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// ToHcsPN
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsPN)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsPN)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionNormal)].Init(
		pDevice,
		VFPositionNormal::GetInputElementDescriptor(),
		VFPositionNormal::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// ToHcsPT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsPT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsPT)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionTexCoord)].Init(
		pDevice,
		VFPositionTexCoord::GetInputElementDescriptor(),
		VFPositionTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// ToHcsPNT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsPNT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsPNT)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionNormalTexCoord)].Init(
		pDevice,
		VFPositionNormalTexCoord::GetInputElementDescriptor(),
		VFPositionNormalTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// ToHcsPNTT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsPNTT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsPNTT)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionNormalTangentTexCoord)].Init(
		pDevice,
		VFPositionNormalTangentTexCoord::GetInputElementDescriptor(),
		VFPositionNormalTangentTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// ToHcsPNTTSkinned
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsPNTTSkinned)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsPNTTSkinned)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::PositionNormalTangentTexCoordSkinned)].Init(
		pDevice,
		VFPositionNormalTangentTexCoordSkinned::GetInputElementDescriptor(),
		VFPositionNormalTangentTexCoordSkinned::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TerrainPatchCtrlPt
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::TerrainPatchCtrlPt)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::TerrainPatchCtrlPt)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::TerrainPatchCtrlPt)].Init(
		pDevice,
		VFTerrainPatchControlPoint::GetInputElementDescriptor(),
		VFTerrainPatchControlPoint::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// TransformSkyboxToHCS (No Input Layout required)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsSkybox)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsSkybox)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ToHcsPNTTQuadForBillboard
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsPNTTQuadForBillboard)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsPNTTQuadForBillboard)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;
	// ┗━ PNTT Input Layout 사용

	// ToHcsPNTTQuadForImage
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsPNTTQuadForImage)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsPNTTQuadForImage)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;
	// ┗━ PNTT Input Layout 사용

	// ToHcsPNTTQuadForShadedEdgeQuad
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsPNTTQuadForShadedEdgeQuad)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsPNTTQuadForShadedEdgeQuad)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;
	// ┗━ PNTT Input Layout 사용

	// ToHcsPNTTQuadForShadedEdgeCircle
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsPNTTQuadForShadedEdgeCircle)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsPNTTQuadForShadedEdgeCircle)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;
	// ┗━ PNTT Input Layout 사용

	// ToHcsCheckbox
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsCheckbox)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsCheckbox)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::Checkbox)].Init(
		pDevice,
		VFCheckbox::GetInputElementDescriptor(),
		VFCheckbox::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;


	
	// Deprecated
	// ToHcsScreenRatioQuad (No Input Layout required)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsScreenRatioQuad)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsScreenRatioQuad)].Init(pDevice, pByteCode, byteCodeSize);
	// No input layout required.
	delete[] pByteCode;


	/*
	// ToHcsShaded2DQuad
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VertexShaderType::ToHcsShaded2DQuad)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VertexShaderType::ToHcsShaded2DQuad)].Init(pDevice, pByteCode, byteCodeSize);
	// ┗━ Create compatible Input Layout
	m_il[static_cast<size_t>(VertexFormatType::Shaded2DQuad)].Init(
		pDevice,
		VFShaded2DQuad::GetInputElementDescriptor(),
		VFShaded2DQuad::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;
	*/








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
	// UnlitP
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::UnlitP)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::UnlitP)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// UnlitPC
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::UnlitPC)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::UnlitPC)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// PSUnlitPNNoMtl
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::UnlitPNNoMtl)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::UnlitPNNoMtl)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// LitPN
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitPN)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitPN)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// UnlitPT1Tex
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::UnlitPT1Tex)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::UnlitPT1Tex)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// UnlitPT1MSTex
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::UnlitPT1MSTex)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::UnlitPT1MSTex)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	/*
	// LitPNT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitPNT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitPNT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;
	*/

	// UnlitPNTTNoMtl
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::UnlitPNTTNoMtl)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::UnlitPNTTNoMtl)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// LitPNTT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitPNTT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitPNTT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// LitPNTTDiffMapping
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitPNTTDiffMapping)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitPNTTDiffMapping)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// LitPNTTSpecMapping
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitPNTTSpecMapping)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitPNTTSpecMapping)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// LitPNTTNormMapping
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitPNTTNormMapping)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitPNTTNormMapping)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// LitPNTTDiffSpecMapping
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitPNTTDiffSpecMapping)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitPNTTDiffSpecMapping)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// LitPNTTDiffNormMapping
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitPNTTDiffNormMapping)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitPNTTDiffNormMapping)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// LitPNTTSpecNormMapping
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitPNTTSpecNormMapping)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitPNTTSpecNormMapping)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// LitPNTTDiffSpecNormMapping
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitPNTTDiffSpecNormMapping)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitPNTTDiffSpecNormMapping)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// SkyboxFragment
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::SkyboxFragment)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::SkyboxFragment)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// LitTerrainFragment
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::LitTerrainFragment)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::LitTerrainFragment)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ColorShadedEdgeQuad
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorShadedEdgeQuad)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorShadedEdgeQuad)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// ColorShadedEdgeCircle
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PixelShaderType::ColorShadedEdgeCircle)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PixelShaderType::ColorShadedEdgeCircle)].Init(pDevice, pByteCode, byteCodeSize);
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

	D3D11_RASTERIZER_DESC rasterizerDesc;

	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	rasterizerDesc.MultisampleEnable = TRUE;
	m_rs[static_cast<size_t>(RasterizerMode::MultisampleWireframe)].Init(
		pDevice, &rasterizerDesc
	);
	rasterizerDesc.MultisampleEnable = FALSE;
	m_rs[static_cast<size_t>(RasterizerMode::Wireframe)].Init(
		pDevice, &rasterizerDesc
	);


	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	rasterizerDesc.MultisampleEnable = TRUE;
	m_rs[static_cast<size_t>(RasterizerMode::MultisampleSolidCullNone)].Init(
		pDevice, &rasterizerDesc
	);
	rasterizerDesc.MultisampleEnable = FALSE;
	m_rs[static_cast<size_t>(RasterizerMode::SolidCullNone)].Init(
		pDevice, &rasterizerDesc
	);


	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;

	rasterizerDesc.MultisampleEnable = TRUE;
	m_rs[static_cast<size_t>(RasterizerMode::MultisampleSolidCullFront)].Init(
		pDevice, &rasterizerDesc
	);
	rasterizerDesc.MultisampleEnable = FALSE;
	m_rs[static_cast<size_t>(RasterizerMode::SolidCullFront)].Init(
		pDevice, &rasterizerDesc
	);


	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;

	rasterizerDesc.MultisampleEnable = TRUE;
	m_rs[static_cast<size_t>(RasterizerMode::MultisampleSolidCullBack)].Init(
		pDevice, &rasterizerDesc
	);
	rasterizerDesc.MultisampleEnable = FALSE;
	m_rs[static_cast<size_t>(RasterizerMode::SolidCullBack)].Init(
		pDevice, &rasterizerDesc
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
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 100000;	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;	(UNORM 깊이 버퍼일 경우 계산식)
	/*
	* if(DepthBiasClamp > 0)
    *	Bias = min(DepthBiasClamp, Bias)
	* else if(DepthBiasClamp < 0)
    *	Bias = max(DepthBiasClamp, Bias)
	*/
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 1.0f;
	// rasterizerDesc.MultisampleEnable = FALSE;
	// rasterizerDesc.AntialiasedLineEnable = FALSE;
	m_rs[static_cast<size_t>(RasterizerMode::ShadowMap)].Init(
		pDevice, &rasterizerDesc
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
	m_dss[static_cast<size_t>(DepthStencilStateType::DepthReadWrite)].Init(pDevice, &depthStencilDesc);

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
	m_dss[static_cast<size_t>(DepthStencilStateType::DepthReadOnly)].Init(pDevice, &depthStencilDesc);

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
	D3D11_RENDER_TARGET_BLEND_DESC& renderTarget0BlendDesc = blendDesc.RenderTarget[0];

	// 1. Opaque
	renderTarget0BlendDesc.BlendEnable = FALSE;
	renderTarget0BlendDesc.SrcBlend = D3D11_BLEND_ONE;
	renderTarget0BlendDesc.DestBlend = D3D11_BLEND_ZERO;
	renderTarget0BlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderTarget0BlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	renderTarget0BlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	renderTarget0BlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderTarget0BlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BlendStateType::Opaque)].Init(pDevice, &blendDesc);

	// 2. AlphaBlend
	renderTarget0BlendDesc.BlendEnable = TRUE;
	renderTarget0BlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	renderTarget0BlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	renderTarget0BlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderTarget0BlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	renderTarget0BlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	renderTarget0BlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderTarget0BlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BlendStateType::AlphaBlend)].Init(pDevice, &blendDesc);

	// 3. No color write
	renderTarget0BlendDesc.BlendEnable = TRUE;
	renderTarget0BlendDesc.SrcBlend = D3D11_BLEND_ZERO;
	renderTarget0BlendDesc.DestBlend = D3D11_BLEND_ONE;
	renderTarget0BlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderTarget0BlendDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
	renderTarget0BlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	renderTarget0BlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderTarget0BlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BlendStateType::NoColorWrite)].Init(pDevice, &blendDesc);
}

void GraphicDevice::ReleaseBlendStates()
{
	for (size_t i = 0; i < static_cast<size_t>(BlendStateType::COUNT); ++i)
		m_bs[i].Release();
}

bool GraphicDevice::CreateCommonGraphicResources()
{
	ID3D11Device* pDevice = this->GetDevice();

	/*
	// UI 렌더링용 음영 2D 사각형 버퍼 생성
	{
		constexpr FLOAT SUNLIT_FACE_COLOR_WEIGHT = 0.5f;
		constexpr FLOAT SHADED_FACE_COLOR_WEIGHT = -0.6f;
		constexpr XMFLOAT2 LT_COLOR_WEIGHTS = XMFLOAT2(SUNLIT_FACE_COLOR_WEIGHT, SHADED_FACE_COLOR_WEIGHT);
		constexpr XMFLOAT2 RB_SHADE_WEIGHTS = XMFLOAT2(LT_COLOR_WEIGHTS.y, LT_COLOR_WEIGHTS.x);	// 좌상단과 음영 반전
		constexpr XMFLOAT2 CENTER_SHADE_WEIGHTS = XMFLOAT2(0.0f, 0.0f);
		constexpr float SHADE_EDGE_THICKNESS = 1.0f;	// 음영 모서리 두께

		VFShaded2DQuad v[SHADED_2DQUAD_VERTEX_COUNT];

		// Top side
		v[0].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[0].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[0].m_colorWeights = LT_COLOR_WEIGHTS;
		v[1].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[1].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[1].m_colorWeights = LT_COLOR_WEIGHTS;
		v[2].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[2].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[2].m_colorWeights = LT_COLOR_WEIGHTS;
		v[3].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[3].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[3].m_colorWeights = LT_COLOR_WEIGHTS;
		v[4].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[4].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[4].m_colorWeights = LT_COLOR_WEIGHTS;
		v[5].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[5].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[5].m_colorWeights = LT_COLOR_WEIGHTS;

		// Left side
		v[6].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[6].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[6].m_colorWeights = LT_COLOR_WEIGHTS;
		v[7].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[7].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[7].m_colorWeights = LT_COLOR_WEIGHTS;
		v[8].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[8].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[8].m_colorWeights = LT_COLOR_WEIGHTS;
		v[9].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[9].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[9].m_colorWeights = LT_COLOR_WEIGHTS;
		v[10].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[10].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[10].m_colorWeights = LT_COLOR_WEIGHTS;
		v[11].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[11].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[11].m_colorWeights = LT_COLOR_WEIGHTS;

		// Right side
		v[12].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[12].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[12].m_colorWeights = RB_SHADE_WEIGHTS;
		v[13].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[13].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[13].m_colorWeights = RB_SHADE_WEIGHTS;
		v[14].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[14].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[14].m_colorWeights = RB_SHADE_WEIGHTS;
		v[15].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[15].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[15].m_colorWeights = RB_SHADE_WEIGHTS;
		v[16].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[16].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[16].m_colorWeights = RB_SHADE_WEIGHTS;
		v[17].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[17].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[17].m_colorWeights = RB_SHADE_WEIGHTS;

		// Bottom side
		v[18].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[18].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[18].m_colorWeights = RB_SHADE_WEIGHTS;
		v[19].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[19].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[19].m_colorWeights = RB_SHADE_WEIGHTS;
		v[20].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[20].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[20].m_colorWeights = RB_SHADE_WEIGHTS;
		v[21].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[21].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[21].m_colorWeights = RB_SHADE_WEIGHTS;
		v[22].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[22].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[22].m_colorWeights = RB_SHADE_WEIGHTS;
		v[23].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[23].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[23].m_colorWeights = RB_SHADE_WEIGHTS;

		// Center
		v[24].m_position = XMFLOAT2(-0.5f, -0.5f);
		v[24].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[24].m_colorWeights = CENTER_SHADE_WEIGHTS;
		v[25].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[25].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[25].m_colorWeights = CENTER_SHADE_WEIGHTS;
		v[26].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[26].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[26].m_colorWeights = CENTER_SHADE_WEIGHTS;
		v[27].m_position = XMFLOAT2(+0.5f, -0.5f);
		v[27].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[27].m_colorWeights = CENTER_SHADE_WEIGHTS;
		v[28].m_position = XMFLOAT2(-0.5f, +0.5f);
		v[28].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[28].m_colorWeights = CENTER_SHADE_WEIGHTS;
		v[29].m_position = XMFLOAT2(+0.5f, +0.5f);
		v[29].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[29].m_colorWeights = CENTER_SHADE_WEIGHTS;

		// Create a vertex buffer
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = sizeof(v);
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = sizeof(v) / _countof(v);

		D3D11_SUBRESOURCE_DATA subrcData;
		ZeroMemory(&subrcData, sizeof(subrcData));
		subrcData.pSysMem = v;
		// initialData.SysMemPitch = 0;		// unused
		// initialData.SysMemSlicePitch = 0;	// unused

		if (!m_vbShaded2DQuad.Init(pDevice, &bufferDesc, &subrcData))
			return false;
	}
	*/

	// Checkbox용 정점 버퍼 생성
	{
		constexpr FLOAT SUNLIT_FACE_COLOR_WEIGHT = 0.5f;
		constexpr FLOAT SHADED_FACE_COLOR_WEIGHT = -0.6f;
		constexpr FLOAT CENTER_SHADE_WEIGHT = 0.0f;
		constexpr float SHADE_EDGE_THICKNESS = 1.0f;	// 음영 모서리 두께
		constexpr XMFLOAT2 V_PRESET[10] =
		{
			XMFLOAT2(-0.5f, 0.5f),		// 0
			XMFLOAT2(0.5f, 0.5f),		// 1
			XMFLOAT2(0.5f, -0.5f),		// 2
			XMFLOAT2(-0.5f, -0.5f),		// 3
			XMFLOAT2(-0.33f, -0.15f),	// 4
			XMFLOAT2(-0.33f, 0.15f),	// 5
			XMFLOAT2(-0.03f, -0.05f),	// 6
			XMFLOAT2(-0.03f, -0.35f),	// 7
			XMFLOAT2(0.35f, 0.0f),		// 8
			XMFLOAT2(0.35f, 0.3f)		// 9
		};

		VFCheckbox v[CHECKBOX_VERTEX_COUNT];

		// Top side
		v[0].m_position = V_PRESET[0];
		v[0].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[0].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[0].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[1].m_position = V_PRESET[1];
		v[1].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[1].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[1].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[2].m_position = V_PRESET[0];
		v[2].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[2].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[2].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[3].m_position = V_PRESET[0];
		v[3].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[3].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[3].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[4].m_position = V_PRESET[1];
		v[4].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[4].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[4].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[5].m_position = V_PRESET[1];
		v[5].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[5].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[5].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// Left side
		v[6].m_position = V_PRESET[0];
		v[6].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[6].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[6].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[7].m_position = V_PRESET[0];
		v[7].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[7].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[7].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[8].m_position = V_PRESET[3];
		v[8].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[8].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[8].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[9].m_position = V_PRESET[3];
		v[9].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[9].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[9].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[10].m_position = V_PRESET[0];
		v[10].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[10].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[10].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[11].m_position = V_PRESET[3];
		v[11].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[11].m_colorWeight = SHADED_FACE_COLOR_WEIGHT;
		v[11].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// Right side
		v[12].m_position = V_PRESET[2];
		v[12].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[12].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[12].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[13].m_position = V_PRESET[1];
		v[13].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[13].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[13].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[14].m_position = V_PRESET[2];
		v[14].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[14].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[14].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[15].m_position = V_PRESET[2];
		v[15].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[15].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[15].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[16].m_position = V_PRESET[1];
		v[16].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[16].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[16].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[17].m_position = V_PRESET[1];
		v[17].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[17].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[17].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// Bottom side
		v[18].m_position = V_PRESET[3];
		v[18].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[18].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[18].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[19].m_position = V_PRESET[3];
		v[19].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[19].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[19].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[20].m_position = V_PRESET[2];
		v[20].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[20].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[20].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[21].m_position = V_PRESET[2];
		v[21].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[21].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[21].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[22].m_position = V_PRESET[3];
		v[22].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[22].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[22].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[23].m_position = V_PRESET[2];
		v[23].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[23].m_colorWeight = SUNLIT_FACE_COLOR_WEIGHT;
		v[23].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// A
		v[24].m_position = V_PRESET[0];
		v[24].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[24].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[24].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[25].m_position = V_PRESET[5];
		v[25].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[25].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[25].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[26].m_position = V_PRESET[3];
		v[26].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[26].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[26].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// B
		v[27].m_position = V_PRESET[3];
		v[27].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[27].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[27].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[28].m_position = V_PRESET[5];
		v[28].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[28].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[28].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[29].m_position = V_PRESET[4];
		v[29].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[29].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[29].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// C
		v[30].m_position = V_PRESET[3];
		v[30].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[30].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[30].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[31].m_position = V_PRESET[4];
		v[31].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[31].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[31].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[32].m_position = V_PRESET[7];
		v[32].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[32].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[32].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// D
		v[33].m_position = V_PRESET[3];
		v[33].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[33].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[33].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[34].m_position = V_PRESET[7];
		v[34].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[34].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[34].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[35].m_position = V_PRESET[2];
		v[35].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[35].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[35].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// E
		v[36].m_position = V_PRESET[2];
		v[36].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[36].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[36].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[37].m_position = V_PRESET[7];
		v[37].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[37].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[37].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[38].m_position = V_PRESET[8];
		v[38].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[38].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[38].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// F
		v[39].m_position = V_PRESET[2];
		v[39].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, +SHADE_EDGE_THICKNESS);
		v[39].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[39].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[40].m_position = V_PRESET[8];
		v[40].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[40].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[40].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[41].m_position = V_PRESET[1];
		v[41].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[41].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[41].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// G
		v[42].m_position = V_PRESET[1];
		v[42].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[42].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[42].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[43].m_position = V_PRESET[8];
		v[43].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[43].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[43].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[44].m_position = V_PRESET[9];
		v[44].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[44].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[44].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// H
		v[45].m_position = V_PRESET[1];
		v[45].m_offset = XMFLOAT2(-SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[45].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[45].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[46].m_position = V_PRESET[9];
		v[46].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[46].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[46].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[47].m_position = V_PRESET[0];
		v[47].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[47].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[47].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// I
		v[48].m_position = V_PRESET[9];
		v[48].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[48].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[48].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[49].m_position = V_PRESET[5];
		v[49].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[49].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[49].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[50].m_position = V_PRESET[0];
		v[50].m_offset = XMFLOAT2(+SHADE_EDGE_THICKNESS, -SHADE_EDGE_THICKNESS);
		v[50].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[50].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// J
		v[51].m_position = V_PRESET[5];
		v[51].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[51].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[51].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[52].m_position = V_PRESET[9];
		v[52].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[52].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[52].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;
		v[53].m_position = V_PRESET[6];
		v[53].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[53].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[53].m_colorIndex = VFCheckbox::BOX_COLOR_INDEX;

		// K
		v[54].m_position = V_PRESET[5];
		v[54].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[54].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[54].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;
		v[55].m_position = V_PRESET[6];
		v[55].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[55].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[55].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;
		v[56].m_position = V_PRESET[4];
		v[56].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[56].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[56].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;

		// L
		v[57].m_position = V_PRESET[4];
		v[57].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[57].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[57].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;
		v[58].m_position = V_PRESET[6];
		v[58].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[58].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[58].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;
		v[59].m_position = V_PRESET[7];
		v[59].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[59].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[59].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;

		// M
		v[60].m_position = V_PRESET[7];
		v[60].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[60].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[60].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;
		v[61].m_position = V_PRESET[6];
		v[61].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[61].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[61].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;
		v[62].m_position = V_PRESET[8];
		v[62].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[62].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[62].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;

		// N
		v[63].m_position = V_PRESET[8];
		v[63].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[63].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[63].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;
		v[64].m_position = V_PRESET[6];
		v[64].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[64].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[64].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;
		v[65].m_position = V_PRESET[9];
		v[65].m_offset = XMFLOAT2(0.0f, 0.0f);
		v[65].m_colorWeight = CENTER_SHADE_WEIGHT;
		v[65].m_colorIndex = VFCheckbox::CHECK_COLOR_INDEX;

		// Create a vertex buffer
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = sizeof(v);
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = sizeof(v) / _countof(v);

		D3D11_SUBRESOURCE_DATA initialData;
		ZeroMemory(&initialData, sizeof(initialData));
		initialData.pSysMem = v;
		// initialData.SysMemPitch = 0;		// unused
		// initialData.SysMemSlicePitch = 0;	// unused

		if (!m_vbCheckbox.Init(pDevice, &bufferDesc, &initialData))
			return false;
	}

	// 빌보드 & UI 렌더링용 쿼드 정점 버퍼 생성
	{
		constexpr XMFLOAT3A NORMALL(0.0f, 0.0f, 1.0f);
		constexpr XMFLOAT3A TANGENTL(-1.0f, 0.0f, 0.0f);
		// const XMFLOAT3A BITANGENTL(0.0f, -1.0f, 0.0f);

		// +Z Forward!
		// TRIANGLESTRIP QUAD
		VFPositionNormalTangentTexCoord v[4];
		v[0].m_position = XMFLOAT3(+0.5f, -0.5f, 0.0f);
		v[0].m_normal = NORMALL;
		v[0].m_tangent = TANGENTL;
		v[0].m_texCoord = XMFLOAT2(0.0f, 1.0f);	// Left bottm (Tex coord system)
		v[1].m_position = XMFLOAT3(+0.5f, +0.5f, 0.0f);
		v[1].m_normal = NORMALL;
		v[1].m_tangent = TANGENTL;
		v[1].m_texCoord = XMFLOAT2(0.0f, 0.0f);	// Left top (Tex coord system)
		v[2].m_position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
		v[2].m_normal = NORMALL;
		v[2].m_tangent = TANGENTL;
		v[2].m_texCoord = XMFLOAT2(1.0f, 1.0f);	// Right bottom (Tex coord system)
		v[3].m_position = XMFLOAT3(-0.5f, +0.5f, 0.0f);
		v[3].m_normal = NORMALL;
		v[3].m_tangent = TANGENTL;
		v[3].m_texCoord = XMFLOAT2(1.0f, 0.0f);	// Right up (Tex coord system)

		// Create a vertex buffer
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = sizeof(v);
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = sizeof(v) / _countof(v);

		D3D11_SUBRESOURCE_DATA initialData;
		ZeroMemory(&initialData, sizeof(initialData));
		initialData.pSysMem = v;
		// initialData.SysMemPitch = 0;		// unused
		// initialData.SysMemSlicePitch = 0;	// unused

		if (!m_vbPNTTQuad.Init(pDevice, &bufferDesc, &initialData))
			return false;
	}


	// Light volume meshes
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
		// initialData.SysMemPitch = 0;		// unused
		// initialData.SysMemSlicePitch = 0;	// unused

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
		// initialData.SysMemPitch = 0;		// unused
		// initialData.SysMemSlicePitch = 0;	// unused
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
		// initialData.SysMemPitch = 0;		// unused
		// initialData.SysMemSlicePitch = 0;	// unused

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
		// initialData.SysMemPitch = 0;		// unused
		// initialData.SysMemSlicePitch = 0;	// unused
		if (!m_slvMeshIB.Init(pDevice, &bufferDesc, &subrcData))
			return false;
	}

	return true;
}

void GraphicDevice::ReleaseCommonGraphicResources()
{
	// m_vbShaded2DQuad.Release();
	m_vbCheckbox.Release();
	m_vbPNTTQuad.Release();
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

	D3D11_RENDER_TARGET_VIEW_DESC swapChainRTVDesc;
	ZeroMemory(&swapChainRTVDesc, sizeof(swapChainRTVDesc));
	swapChainRTVDesc.Format = SWAP_CHAIN_RTV_FORMAT;	// sRGB
	swapChainRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	swapChainRTVDesc.Texture2D.MipSlice = 0;

	ComPtr<ID3D11RenderTargetView> cpRTVSwapChain;
	hr = m_cpDevice->CreateRenderTargetView(cpBackBuffer.Get(), &swapChainRTVDesc, cpRTVSwapChain.GetAddressOf());
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
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
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

	ComPtr<ID3D11DepthStencilView> cpDSVSwapChain;
	hr = m_cpDevice->CreateDepthStencilView(cpDepthStencilBuffer.Get(), nullptr, cpDSVSwapChain.GetAddressOf());
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


	// Create a Direct2D render target that can draw into the surface in the swap chain
	const D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);
	ComPtr<ID2D1RenderTarget> cpD2DRTBackBuffer;
	hr = m_cpD2DFactory->CreateDxgiSurfaceRenderTarget(cpBackBufferSurface.Get(), &props, cpD2DRTBackBuffer.GetAddressOf());
	
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID2D1Factory::CreateDxgiSurfaceRenderTarget", hr);
		return false;
	}

	ComPtr<ID2D1SolidColorBrush> cpD2DSolidColorBrush;
	hr = cpD2DRTBackBuffer->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), cpD2DSolidColorBrush.GetAddressOf());
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID2D1RenderTarget::CreateSolidColorBrush", hr);
		return false;
	}

	m_cpRTVSwapChain = std::move(cpRTVSwapChain);
	m_cpDSVSwapChain = std::move(cpDSVSwapChain);
	m_cpD2DRTBackBuffer = std::move(cpD2DRTBackBuffer);
	m_cpD2DSolidColorBrush = std::move(cpD2DSolidColorBrush);

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
	m_cpD2DRTBackBuffer.Reset();
	// D3D 리소스들
	m_cpDSVSwapChain.Reset();
	m_cpRTVSwapChain.Reset();
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
