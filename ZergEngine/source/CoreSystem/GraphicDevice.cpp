#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\RenderState.h>

using namespace ze;

GraphicDevice* GraphicDevice::s_pInstance = nullptr;

PCWSTR SHADER_LOAD_FAIL_MSG_FMT = L"Failed to open compiled shader object.\n%s";
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
	L"VSTransformButtonToHCS.cso",
	L"VSTransformPTQuadToHCS.cso"
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
	L"PSTexturePTFragment.cso"
};

// constexpr float allows only one floating point constant to exist in memory, even if it is not encoded in a x86 command.
constexpr uint32_t SWAP_CHAIN_FLAG = 0;
constexpr DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM;

GraphicDevice::GraphicDevice()
	: m_backBufferFormat(BACK_BUFFER_FORMAT)
	, m_descAdapter()
	, m_descSwapChain()
	, m_swapChainSizeFlt(0.0f, 0.0f)
	, m_swapChainHalfSizeFlt(0.0f, 0.0f)
	, m_descDepthStencil()
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
	, m_vb{}
	, m_rs{}
	, m_ss{}
	, m_dss{}
	, m_bs{}
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

void GraphicDevice::Init(HWND hWnd, uint32_t width, uint32_t height, bool fullscreen)
{
	HRESULT hr;

	// A. D3D11 ����̽� �� ���ؽ�Ʈ ����
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
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"D3D11CreateDevice()", hr);

	if (maxSupportedFeatureLevel < D3D_FEATURE_LEVEL_11_1)
		Debug::ForceCrashWithMessageBox(L"Fail", L"Device does not support DirectX 11.1 feature level.");

	// B. D2D Factory ����
	assert(m_cpD2DFactory == nullptr);
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), reinterpret_cast<void**>(m_cpD2DFactory.GetAddressOf()));
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"D2D1CreateFactory()", hr);

	// C. DWrite Factory ���� �� �⺻ �ؽ�Ʈ���� ����
	assert(m_cpDWriteFactory == nullptr);
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(m_cpDWriteFactory.GetAddressOf()));
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"DWriteCreateFactory()", hr);


	// �����Ǵ� �ػ� ��� �˻�
	this->CreateSupportedResolutionInfo();

	// �ϵ����� �����ϴ� ��Ƽ���ø� ī��Ʈ üũ
	// FEATURE_LEVEL_11_0 devices are required to support 4x MSAA for all render target formats,
	// and 8x MSAA for all render target formats except R32G32B32A32 formats.
	this->CreateSupportedMSAAQualityInfo();

	// ���� ü�� ��ũ���� �� �������ٽ� ���� ��ũ���� �ʱ�ȭ
	// Initialize SwapChain descriptor.
	DXGI_SWAP_CHAIN_DESC descSwapChain;
	ZeroMemory(&descSwapChain, sizeof(descSwapChain));
	descSwapChain.BufferDesc.Width = width;
	descSwapChain.BufferDesc.Height = height;
	descSwapChain.BufferDesc.RefreshRate.Numerator = 1;
	descSwapChain.BufferDesc.RefreshRate.Denominator = 144;
	descSwapChain.BufferDesc.Format = this->GetBackBufferFormat();
	descSwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	descSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	descSwapChain.SampleDesc.Count = 1;
	descSwapChain.SampleDesc.Quality = 0;
	descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descSwapChain.BufferCount = 2;
	descSwapChain.OutputWindow = hWnd;
	descSwapChain.Windowed = TRUE;		// ���ʿ��� â ��� ����ü���� �����ϴ� ������ (MSDN ���� �����ؼ�..)
	descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// Windows 10���� ���� (This flag cannot be used with multisampling and partial presentation.)
	descSwapChain.Flags = SWAP_CHAIN_FLAG;

	D3D11_TEXTURE2D_DESC descDepthStencil;
	ZeroMemory(&descDepthStencil, sizeof(descDepthStencil));
	// Depth/Stencil buffer (���� ũ��� �ݵ�� ���� ü���� �� ���ۿ� ��ġ�ؾ� ��)
	descDepthStencil.Width = descSwapChain.BufferDesc.Width;
	descDepthStencil.Height = descSwapChain.BufferDesc.Height;
	descDepthStencil.MipLevels = 1;
	descDepthStencil.ArraySize = 1;
	descDepthStencil.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// Must match with swap chain's MSAA setting!
	descDepthStencil.SampleDesc = descSwapChain.SampleDesc;
	descDepthStencil.Usage = D3D11_USAGE_DEFAULT;
	descDepthStencil.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepthStencil.CPUAccessFlags = 0;
	descDepthStencil.MiscFlags = 0;

	// ���� ü�� ����
	// First, Get IDXGIFactory instance
	// DirectX Graphics Infrastructure(DXGI)
	ComPtr<IDXGIDevice> cpDXGIDevice;
	ComPtr<IDXGIAdapter> cpDXGIAdapter;
	ComPtr<IDXGIFactory> cpDXGIFactory;

	// Use the IDXGIFactory instance that was used to create the device! (by COM queries)
	// Device ��ü�� ����� IDXGIFactory �������̽� ��ü�� ȹ���ؼ� ���� ü���� �����ؾ� �Ѵ�.
	do
	{
		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		hr = m_cpDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(cpDXGIDevice.GetAddressOf()));
		if (FAILED(hr))
			break;

		hr = cpDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(cpDXGIAdapter.GetAddressOf()));
		if (FAILED(hr))
			break;

		// ����� ���� ȹ��
		hr = cpDXGIAdapter->GetDesc(&m_descAdapter);
		if (FAILED(hr))
			break;

		hr = cpDXGIAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(cpDXGIFactory.GetAddressOf()));
		if (FAILED(hr))
			break;

		// ���� ü�� ����
		assert(m_cpSwapChain == nullptr);
		hr = cpDXGIFactory->CreateSwapChain(m_cpDevice.Get(), &descSwapChain, m_cpSwapChain.GetAddressOf());
		if (FAILED(hr))
			break;

		// ������� ���� ü���� ���� ����
		m_cpSwapChain->GetDesc(&m_descSwapChain);
		m_swapChainSizeFlt = XMFLOAT2(static_cast<FLOAT>(m_descSwapChain.BufferDesc.Width), static_cast<FLOAT>(m_descSwapChain.BufferDesc.Height));
		m_swapChainHalfSizeFlt = XMFLOAT2(m_swapChainSizeFlt.x * 0.5f, m_swapChainSizeFlt.y * 0.5f);
		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		
		
		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		// ���� ü���� ����ۿ� ���� ���� Ÿ�� �� �����
		ComPtr<ID3D11Texture2D> cpBackBuffer;
		hr = m_cpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(cpBackBuffer.GetAddressOf()));
		if (FAILED(hr))
			break;

		assert(m_cpSwapChainRTV == nullptr);
		hr = m_cpDevice->CreateRenderTargetView(cpBackBuffer.Get(), nullptr, m_cpSwapChainRTV.GetAddressOf());
		if (FAILED(hr))
			break;
		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		// ���� ü�ο� ���� �������� ����� ���� ���ٽ� �� �����
		ComPtr<ID3D11Texture2D> cpDepthStencilBuffer;
		hr = m_cpDevice->CreateTexture2D(&descDepthStencil, nullptr, cpDepthStencilBuffer.GetAddressOf());
		if (FAILED(hr))
			break;
		cpDepthStencilBuffer->GetDesc(&m_descDepthStencil);

		assert(m_cpSwapChainDSV == nullptr);
		hr = m_cpDevice->CreateDepthStencilView(cpDepthStencilBuffer.Get(), nullptr, m_cpSwapChainDSV.GetAddressOf());
		if (FAILED(hr))
			break;
		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		
		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		// D2D ���ҽ� ����
		// 
		// ���� ü���� �� ���ۿ� ���� Direct2D�� DXGI Surface ���� Ÿ�� ����
		ComPtr<IDXGISurface> cpBackBufferSurface;
		hr = m_cpSwapChain->GetBuffer(0, IID_PPV_ARGS(cpBackBufferSurface.GetAddressOf()));
		if (FAILED(hr))
			break;

		assert(m_cpD2DRenderTarget == nullptr);
		const D2D1_RENDER_TARGET_PROPERTIES props =	D2D1::RenderTargetProperties(
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
			break;

		assert(m_cpD2DSolidColorBrush == nullptr);
		hr = m_cpD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), m_cpD2DSolidColorBrush.GetAddressOf());
		if (FAILED(hr))
			break;
	} while (false);

	this->UpdateEntireSwapChainViewport(m_descSwapChain.BufferDesc.Width, m_descSwapChain.BufferDesc.Height);

	// DXGI ���Ǳ�� ��Ȱ��ȭ
	hr = cpDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"MakeWindowAssociation()", hr);

	// �������������������������������������������������� CREATE SHADERS AND INPUT LAYOUTS ��������������������������������������������������
	this->CreateShaderAndInputLayout();

	// ���������������������������������������������������������� CREATE RASTERIZER STATES ����������������������������������������������������������
	this->CreateRasterizerStates();

	// �������������������������������������������������������������� CREATE SAMPLER STATES ������������������������������������������������������������
	this->CreateSamplerStates();

	// �������������������������������������������������������� CREATE DEPTHSTENCIL STATES ��������������������������������������������������������
	this->CreateDepthStencilStates();

	// ���������������������������������������������������������������� CREATE BLEND STATES ��������������������������������������������������������������
	this->CreateBlendStates();

	// ������������������������������������������������������ CREATE COMMON VERTEX BUFFERS ������������������������������������������������������
	this->CreateCommonVertexBuffers();
}

void GraphicDevice::UnInit()
{
	HRESULT hr;

	// ���������������������������������������������������� RELEASE COMMON VERTEX BUFFERS ������������������������������������������������������
	this->ReleaseCommonVertexBuffers();

	// �������������������������������������������������������������� RELEASE BLEND STATES ��������������������������������������������������������������
	this->ReleaseBlendStates();

	// ������������������������������������������������������ RELEASE DEPTHSTENCIL STATES ��������������������������������������������������������
	this->ReleaseDepthStencilStates();

	// ������������������������������������������������������������ RELEASE SAMPLER STATES ������������������������������������������������������������
	this->ReleaseSamplerStates();

	// �������������������������������������������������������� RELEASE RASTERIZER STATES ����������������������������������������������������������
	this->ReleaseRasterizerStates();

	// ������������������������������������������������ RELEASE SHADERS AND INPUT LAYOUTS ��������������������������������������������������
	this->ReleaseShaderAndInputLayout();

	// m_supportedMSAA.clear();		// �ʼ� X

	// ��üȭ�� ��忴�ٸ� ���� ü�� ���� ���� â���� ��������� ũ���� ���� ����
	// https://learn.microsoft.com/ko-kr/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi#destroying-a-swap-chain
	// '���� ü�� ����' �׸� ����
	if (m_cpSwapChain != nullptr)
	{
		BOOL isFullscreen;
		hr = this->GetFullscreenState(&isFullscreen);
		if (SUCCEEDED(hr) && isFullscreen != FALSE)
			m_cpSwapChain->SetFullscreenState(FALSE, nullptr);
	}

	m_fontMap.clear();	// ��� IDWriteTextFormat COM ��ü ����
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
	FILE* pCSOFile = nullptr;
	WCHAR filePath[MAX_PATH];

	HRESULT hr;
	hr = FileSystem::GetInstance()->RelativePathToFullPath(path, filePath, sizeof(filePath));
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

	// ����Ʈ �ڵ�� ũ�� ����
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
	// 1. TRANSFORM_SKYBOX_TO_HCS (No Input Layout required)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_SKYBOX_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_SKYBOX_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 2. TRANSFORM_TERRAIN_PATCH_CTRL_PT (POSITION, TEXCOORD0, TEXCOORD1)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_TERRAIN_PATCH_CTRL_PT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_TERRAIN_PATCH_CTRL_PT)].Init(pDevice, pByteCode, byteCodeSize);
	// ���� Create compatible Input Layout
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
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_P_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ���� Create compatible Input Layout
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
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PC_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ���� Create compatible Input Layout
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
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PN_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ���� Create compatible Input Layout
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
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PT_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ���� Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_TEXCOORD)].Init(
		pDevice,
		VFPositionTexCoord::GetInputElementDescriptor(),
		VFPositionTexCoord::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 7. TRANSFORM_CAMERA_MERGE_QUAD (No Input Layout required)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_CAMERA_MERGE_QUAD)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_CAMERA_MERGE_QUAD)].Init(pDevice, pByteCode, byteCodeSize);
	// No input layout required.
	delete[] pByteCode;

	// 8. TRANSFORM_PNT_TO_HCS (POSITION, NORMAL, TEXCOORD)
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PNT_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PNT_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ���� Create compatible Input Layout
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
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_BUTTON_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	// ���� Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::BUTTON)].Init(
		pDevice,
		VFButton::GetInputElementDescriptor(),
		VFButton::GetInputElementCount(),
		pByteCode,
		byteCodeSize
	);
	delete[] pByteCode;

	// 10. TRANSFORM_PT_QUAD_TO_HCS
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), VERTEX_SHADER_FILES[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PT_QUAD_TO_HCS)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TRANSFORM_PT_QUAD_TO_HCS)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// HULL SHADERS
	// 1. HS CALC_TERRAIN_TESS_FACTOR
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), HULL_SHADER_FILES[static_cast<size_t>(HULL_SHADER_TYPE::CALC_TERRAIN_TESS_FACTOR)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_hs[static_cast<size_t>(HULL_SHADER_TYPE::CALC_TERRAIN_TESS_FACTOR)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// DOMAIN SHADERS
	// 1. DS SAMPLE_TERRAIN_HEIGHT_MAP
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), DOMAIN_SHADER_FILES[static_cast<size_t>(DOMAIN_SHADER_TYPE::SAMPLE_TERRAIN_HEIGHT_MAP)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ds[static_cast<size_t>(DOMAIN_SHADER_TYPE::SAMPLE_TERRAIN_HEIGHT_MAP)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// PIXEL SHADERS
	// 1. COLOR_SKYBOX_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_SKYBOX_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_SKYBOX_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 2. COLOR_TERRAIN_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_TERRAIN_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_TERRAIN_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 3. COLOR_P_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_P_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_P_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 4. COLOR_PC_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PC_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PC_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 5. COLOR_PN_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PN_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PN_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 6. COLOR_PT_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 7. COLOR_PNT_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PNT_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PNT_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 8. COLOR_PT_FRAGMENT_SINGLE_TEXTURE (Not implemented)
	/*
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_TEXTURE)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_TEXTURE)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;
	*/

	// 9. COLOR_PT_FRAGMENT_SINGLE_MSTEXTURE
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_MSTEXTURE)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_MSTEXTURE)].Init(pDevice, pByteCode, byteCodeSize);
	delete[] pByteCode;

	// 10. TEXTURE_PT_FRAGMENT
	StringCbCopyW(targetPath, sizeof(targetPath), SHADER_PATH);
	StringCbCatW(targetPath, sizeof(targetPath), PIXEL_SHADER_FILES[static_cast<size_t>(PIXEL_SHADER_TYPE::TEXTURE_PT_FRAGMENT)]);
	if (!LoadShaderByteCode(targetPath, &pByteCode, &byteCodeSize))
		Debug::ForceCrashWithMessageBox(L"Error", SHADER_LOAD_FAIL_MSG_FMT, targetPath);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::TEXTURE_PT_FRAGMENT)].Init(pDevice, pByteCode, byteCodeSize);
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

void GraphicDevice::ReleaseRasterizerStates()
{
	for (size_t i = 0; i < static_cast<size_t>(RASTERIZER_FILL_MODE::COUNT); ++i)
		for (size_t j = 0; j < static_cast<size_t>(RASTERIZER_CULL_MODE::COUNT); ++j)
			m_rs[i][j].Release();
}

void GraphicDevice::CreateSamplerStates()
{
	ID3D11Device* pDevice = m_cpDevice.Get();

	D3D11_SAMPLER_DESC samplerDesc;

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;		// ����Ʈ ���ø�
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::POINT)].Init(pDevice, &samplerDesc);

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; // �ּ��� (Bilinear)
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::BILINEAR)].Init(pDevice, &samplerDesc);

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // �Ｑ�� (Trilinear)
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::TRILINEAR)].Init(pDevice, &samplerDesc);

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	samplerDesc.MaxAnisotropy = 2;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_2X)].Init(pDevice, &samplerDesc);

	samplerDesc.MaxAnisotropy = 4;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X)].Init(pDevice, &samplerDesc);

	samplerDesc.MaxAnisotropy = 8;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_8X)].Init(pDevice, &samplerDesc);

	samplerDesc.MaxAnisotropy = 16;
	m_ss[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_16X)].Init(pDevice, &samplerDesc);
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
	// 1. Default
	depthStencilDesc.DepthEnable = TRUE;							// ���� �׽�Ʈ Ȱ��ȭ
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// ���� �� ��� ���
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;				// �� ����� �ȼ��� �׸���
	depthStencilDesc.StencilEnable = FALSE;							// ���ٽ� ��Ȱ��ȭ
	m_dss[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::DEFAULT)].Init(pDevice, &depthStencilDesc);

	// 2. ��ī�̹ڽ� ������
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	// ��ī�̹ڽ� ������ �� �߿�
	depthStencilDesc.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::SKYBOX)].Init(pDevice, &depthStencilDesc);

	// 3. �ſ� ��������
	// ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	// ...

	// 4. Depth read only
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::DEPTH_READ_ONLY)].Init(pDevice, &depthStencilDesc);

	// 5. No Depth/Stencil test (ī�޶� ���� ��...)
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.StencilEnable = FALSE;
	m_dss[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::NO_DEPTH_STENCIL_TEST)].Init(pDevice, &depthStencilDesc);
}

void GraphicDevice::ReleaseDepthStencilStates()
{
	for (size_t i = 0; i < _countof(m_dss); ++i)
		m_dss[i].Release();
}

void GraphicDevice::CreateBlendStates()
{
	ID3D11Device* pDevice = m_cpDevice.Get();

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
	m_bs[static_cast<size_t>(BLEND_STATE_TYPE::OPAQUE_)].Init(pDevice, &descBlend);

	// 2. AlphaBlend
	descRenderTargetBlend.BlendEnable = TRUE;
	descRenderTargetBlend.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	descRenderTargetBlend.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	
	descRenderTargetBlend.BlendOp = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.SrcBlendAlpha = D3D11_BLEND_ONE;
	descRenderTargetBlend.DestBlendAlpha = D3D11_BLEND_ZERO;
	descRenderTargetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BLEND_STATE_TYPE::ALPHABLEND)].Init(pDevice, &descBlend);

	// 3. No color write
	descRenderTargetBlend.BlendEnable = TRUE;
	descRenderTargetBlend.SrcBlend = D3D11_BLEND_ZERO;
	descRenderTargetBlend.DestBlend = D3D11_BLEND_ONE;
	descRenderTargetBlend.BlendOp = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.SrcBlendAlpha = D3D11_BLEND_ZERO;
	descRenderTargetBlend.DestBlendAlpha = D3D11_BLEND_ONE;
	descRenderTargetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	descRenderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_bs[static_cast<size_t>(BLEND_STATE_TYPE::NO_COLOR_WRITE)].Init(pDevice, &descBlend);
}

void GraphicDevice::ReleaseBlendStates()
{
	for (size_t i = 0; i < static_cast<size_t>(BLEND_STATE_TYPE::COUNT); ++i)
		m_bs[i].Release();
}

UINT GraphicDevice::GetMSAAMaximumQuality(MSAA_SAMPLE_COUNT sampleCount)
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

void GraphicDevice::UpdateEntireSwapChainViewport(uint32_t width, uint32_t height)
{
	m_entireSwapChainViewport.TopLeftX = 0.0f;
	m_entireSwapChainViewport.TopLeftY = 0.0f;
	m_entireSwapChainViewport.Width = static_cast<FLOAT>(width);
	m_entireSwapChainViewport.Height = static_cast<FLOAT>(height);
	m_entireSwapChainViewport.MinDepth = 0.0f;
	m_entireSwapChainViewport.MaxDepth = 1.0f;
}

std::shared_ptr<DWriteTextFormatWrapper> GraphicDevice::GetDWriteTextFormatWrapper(const TextFormat& tf)
{
	HRESULT hr;
	std::shared_ptr<DWriteTextFormatWrapper> spReturn;
	std::unordered_map<TextFormat, std::weak_ptr<DWriteTextFormatWrapper>, TextFormatHasher>::iterator iter = m_fontMap.find(tf);
	bool createFont;
	bool reuseNode = false;

	if (iter != m_fontMap.end())	// ��尡 �����ϴ� ���
	{
		std::weak_ptr<DWriteTextFormatWrapper> wpTextFormat = iter->second;
		if (!wpTextFormat.expired())	// �����͵� ����ִ� ���
		{
			createFont = false;
			spReturn = wpTextFormat.lock();
		}
		else	// ���� ���������� �����ʹ� ����� ���
		{
			// �ؽø� ���� ���� �����ϹǷ� second�� weak_ptr�� �ٽ� ����ִ� �����͸� �������ش�.
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
		// ��Ʈ �� �ؽø� ��� ���� ����
		// ���� ����
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
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDWriteFactory::CreateTextFormat()", hr);

		spReturn = std::make_shared<DWriteTextFormatWrapper>(pTextFormat);

		if (reuseNode)
		{
			iter->second = spReturn;
		}
	}

	return spReturn;
}

void GraphicDevice::CreateSupportedResolutionInfo()
{
	// DirectX �� DXGI �ʱ�ȭ
	ComPtr<IDXGIFactory> cpFactory;
	HRESULT hr;

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(cpFactory.GetAddressOf()));
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"CreateSupportedResolutionInfo() > CreateDXGIFactory()", hr);

	// ù ��° ����͸� �������� (���� GPU ����)
	ComPtr<IDXGIAdapter> cpAdapter;
	hr = cpFactory->EnumAdapters(0, cpAdapter.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"CreateSupportedResolutionInfo() > IDXGIFactory::EnumAdapters()", hr);

	// ù ��° ����� ����
	ComPtr<IDXGIOutput> cpOutput;
	hr = cpAdapter->EnumOutputs(0, cpOutput.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"CreateSupportedResolutionInfo() > IDXGIAdapter::EnumOutputs()", hr);

	// �����Ǵ� �ػ� ��� ��������
	// ���� ���� ȹ��
	UINT numModes = 0;
	hr = cpOutput->GetDisplayModeList(this->GetBackBufferFormat(), 0, &numModes, nullptr);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Failed to load display mode list.", hr);

	m_supportedResolution.resize(numModes);
	hr = cpOutput->GetDisplayModeList(this->GetBackBufferFormat(), 0, &numModes, m_supportedResolution.data());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Failed to load display mode list.", hr);
}

void GraphicDevice::CreateSupportedMSAAQualityInfo()
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
		hr = m_cpDevice->CheckMultisampleQualityLevels(this->GetBackBufferFormat(), static_cast<UINT>(sc[i]), &quality);

		if (SUCCEEDED(hr) && quality != 0)
			m_supportedMSAA.push_back(std::make_pair(sc[i], quality - 1));
	}
}

void GraphicDevice::CreateCommonVertexBuffers()
{
	ID3D11Device* pDevice = m_cpDevice.Get();

	// 1. Button VB
	{
		const XMFLOAT2 ltShade = XMFLOAT2(+0.5f, -0.5f);			// ���� ó����
		const XMFLOAT2 rbShade = XMFLOAT2(ltShade.y, ltShade.x);	// ���� ó����
		const XMFLOAT2 centerShade = XMFLOAT2(0.0f, 0.0f);
		const float offsetValue = 2.0f;		// ��ư ��ü�� �𼭸� ������
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
		descBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		descBuffer.CPUAccessFlags = 0;
		descBuffer.MiscFlags = 0;
		descBuffer.StructureByteStride = sizeof(VFButton);

		D3D11_SUBRESOURCE_DATA sbrcBuffer;
		ZeroMemory(&sbrcBuffer, sizeof(sbrcBuffer));
		sbrcBuffer.pSysMem = v;
		// sbrcBuffer.SysMemPitch = 0;		// unused
		// sbrcBuffer.SysMemSlicePitch = 0;	// unused

		m_vb[static_cast<size_t>(VERTEX_BUFFER_TYPE::BUTTON)].Init(pDevice, &descBuffer, &sbrcBuffer);
	}
}

void GraphicDevice::ReleaseCommonVertexBuffers()
{
	for (size_t i = 0; i < static_cast<size_t>(VERTEX_BUFFER_TYPE::COUNT); ++i)
		m_vb[i].Release();
}

void GraphicDevice::ResizeBuffer(uint32_t width, uint32_t height)
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

	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	// https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
	// [Remarks] MSDN ����
	// You can't resize a swap chain unless you release all outstanding references to its back buffers. <- �߿�
	// 
	// You must release all of its 'direct' and 'indirect' references on the back buffers in order for ResizeBuffers to succeed.
	// (���� ����)
	// 'Direct references' are held by the application after it calls AddRef on a resource.
	// 
	// (���� ����)
	// 'Indirect references' are held by views to a resource, binding a view of the resource to a device context,
	// a command list that used the resource, a command list that used a view to that resource,
	// a command list that executed another command list that used the resource, and so on.

	// ���� ü�ο� ���� ���ҽ��� ��� ����
	// D2D ���ҽ���
	m_cpD2DSolidColorBrush.Reset();
	m_cpD2DRenderTarget.Reset();
	// D3D ���ҽ���
	m_cpSwapChainDSV.Reset();
	m_cpSwapChainRTV.Reset();
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

	// ��� ����
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	// ���÷��̰� �����ϴ� �ػ󵵿��߸� �Ѵ�. (����ڿ��� �����ϴ� ��� ���� �ʿ�)
	// MSDN
	// â ����� ���� ü���� �����ϰ� ���� ����ڰ� SetFullscreenState�� ���� ���� ü���� ��ü ȭ������ ������ �� �ֵ��� �ϴ� ���� �����ϴ�.
	// ��, ���� ü���� ��ü ȭ������ ������ �����ϱ� ���� DXGI_SWAP_CHAIN_DESC �� Windowed ����� FALSE�� �������� ���ʽÿ�.
	// �׷��� ���� ü���� ��ü ȭ������ �����ϴ� ��� ���� ����ڿ��� �����Ǵ� ���÷��� ��� ��ϵ� �����ؾ� �մϴ�.
	// �������� �ʴ� ���÷��� ���� ������ ���� ü���� ���÷��̰� �˰� ���ϰ� ���� ����ڰ� �ƹ��͵� �� �� ���� ���� �� �ֱ� �����Դϴ�. (�߿�!)
	// ���� ���� ����ڰ� ���÷��� ��带 ������ �� �ֵ��� ����� �� �ð� �ʰ� Ȯ�� ȭ���̳� ��Ÿ ��ü ��Ŀ������ ���� ���� �����ϴ�.
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

	// ����ü�� ũ�� ����
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	// m_pSwapChain->Resize_target();
	hr = m_cpSwapChain->ResizeBuffers(
		0,							// Set this number to zero to preserve the existing number of buffers in the swap chain.
		width,
		height,
		DXGI_FORMAT_UNKNOWN,		// Set this value to DXGI_FORMAT_UNKNOWN to preserve the existing format of the back buffer.
		SWAP_CHAIN_FLAG
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::ResizeBuffers", hr);

	// ����� ���� ü���� ���� ȹ��
	hr = m_cpSwapChain->GetDesc(&m_descSwapChain);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::GetDesc", hr);

	m_swapChainSizeFlt = XMFLOAT2(static_cast<FLOAT>(m_descSwapChain.BufferDesc.Width), static_cast<FLOAT>(m_descSwapChain.BufferDesc.Height));
	m_swapChainHalfSizeFlt = XMFLOAT2(m_swapChainSizeFlt.x * 0.5f, m_swapChainSizeFlt.y * 0.5f);

	// �������ٽ� ���� ������� ���� ��ũ���� ����
	D3D11_TEXTURE2D_DESC descDepthStencil;
	ZeroMemory(&descDepthStencil, sizeof(descDepthStencil));
	descDepthStencil.Width = m_descSwapChain.BufferDesc.Width;
	descDepthStencil.Height = m_descSwapChain.BufferDesc.Height;
	descDepthStencil.MipLevels = 1;
	descDepthStencil.ArraySize = 1;
	descDepthStencil.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// Must match with swap chain's MSAA setting!
	descDepthStencil.SampleDesc = m_descSwapChain.SampleDesc;
	descDepthStencil.Usage = D3D11_USAGE_DEFAULT;
	descDepthStencil.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepthStencil.CPUAccessFlags = 0;
	descDepthStencil.MiscFlags = 0;

	do
	{
		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		// D3D ���ҽ� �����
		// ���� ü���� ����ۿ� ���� ���� Ÿ�� �� �����
		ComPtr<ID3D11Texture2D> cpBackBuffer;
		hr = m_cpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(cpBackBuffer.GetAddressOf()));
		if (FAILED(hr))
			break;

		assert(m_cpSwapChainRTV == nullptr);
		hr = m_cpDevice->CreateRenderTargetView(cpBackBuffer.Get(), nullptr, m_cpSwapChainRTV.GetAddressOf());
		if (FAILED(hr))
			break;

		// ���� ü�ο� ���� �������� ����� ���� ���ٽ� ���� �� �� �����
		ComPtr<ID3D11Texture2D> cpDepthStencilBuffer;	// DSV�� ���� ���ٽ� ���۸� ���� �����ϹǷ� ���� ComPtr�� �ξ ��
		hr = m_cpDevice->CreateTexture2D(&descDepthStencil, nullptr, cpDepthStencilBuffer.GetAddressOf());
		if (FAILED(hr))
			break;

		assert(m_cpSwapChainDSV == nullptr);
		cpDepthStencilBuffer->GetDesc(&m_descDepthStencil);
		hr = m_cpDevice->CreateDepthStencilView(cpDepthStencilBuffer.Get(), nullptr, m_cpSwapChainDSV.GetAddressOf());
		if (FAILED(hr))
			break;
		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		// D2D ���ҽ� �����
		// ���� ü���� �� ���ۿ� ���� Direct2D�� DXGI Surface ���� Ÿ�� ����
		ComPtr<IDXGISurface> cpBackBufferSurface;
		hr = m_cpSwapChain->GetBuffer(0, IID_PPV_ARGS(cpBackBufferSurface.GetAddressOf()));
		if (FAILED(hr))
			break;

		assert(m_cpD2DRenderTarget == nullptr);
		const D2D1_RENDER_TARGET_PROPERTIES props =	D2D1::RenderTargetProperties(
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
			break;

		assert(m_cpD2DSolidColorBrush == nullptr);
		hr = m_cpD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), m_cpD2DSolidColorBrush.GetAddressOf());
		if (FAILED(hr))
			break;
		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	} while (false);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Failed to resize back buffer!", hr);
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

	// ��ü ���� ü�� ������ ��Ÿ���� ����Ʈ ����ü ������Ʈ
	this->UpdateEntireSwapChainViewport(m_descSwapChain.BufferDesc.Width, m_descSwapChain.BufferDesc.Height);
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
