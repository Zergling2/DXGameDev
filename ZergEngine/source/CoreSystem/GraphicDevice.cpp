#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\Common\EngineConstants.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\FileSystem.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(GraphicDevice);

// constexpr float allows only one floating point constant to exist in memory, even if it is not encoded in a x86 command.
constexpr DXGI_FORMAT BACKBUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr uint32_t SWAP_CHAIN_FLAG = 0;

static PCWSTR ENGINEBIN_CSO_VS_PATH[static_cast<size_t>(VERTEX_SHADER_TYPE::COUNT)] =
{
	L"Engine\\Bin\\CSO\\VSSkyboxTransform.cso",
	L"Engine\\Bin\\CSO\\VSTerrainTransform.cso",
	L"Engine\\Bin\\CSO\\VSStandardTransformP.cso",
	L"Engine\\Bin\\CSO\\VSStandardTransformPC.cso",
	L"Engine\\Bin\\CSO\\VSStandardTransformPN.cso",
	L"Engine\\Bin\\CSO\\VSStandardTransformPT.cso",
	L"Engine\\Bin\\CSO\\VSRenderingResultMerge.cso",
	L"Engine\\Bin\\CSO\\VSStandardTransformPNT.cso"
};
static PCWSTR ENGINEBIN_CSO_HS_PATH[static_cast<size_t>(HULL_SHADER_TYPE::COUNT)] =
{
	L"Engine\\Bin\\CSO\\HSTerrainRendering.cso"
};
static PCWSTR ENGINEBIN_CSO_DS_PATH[static_cast<size_t>(DOMAIN_SHADER_TYPE::COUNT)] =
{
	L"Engine\\Bin\\CSO\\DSTerrainRendering.cso"
};
static PCWSTR ENGINEBIN_CSO_PS_PATH[static_cast<size_t>(PIXEL_SHADER_TYPE::COUNT)] =
{
	L"Engine\\Bin\\CSO\\PSSkyboxColoring.cso",
	L"Engine\\Bin\\CSO\\PSTerrainColoring.cso",
	L"Engine\\Bin\\CSO\\PSSkyboxColoring.cso",
	L"Engine\\Bin\\CSO\\PSStandardColoringPC.cso",
	L"Engine\\Bin\\CSO\\PSStandardColoringPN.cso",
	L"Engine\\Bin\\CSO\\PSStandardColoringPT.cso",
	L"Engine\\Bin\\CSO\\PSRenderingResultMerge_Unimplemented.cso",		// ����
	L"Engine\\Bin\\CSO\\PSMSRenderingResultMerge.cso",
	L"Engine\\Bin\\CSO\\PSStandardColoringPNT.cso"
};

GraphicDevice::GraphicDevice()
	: m_descAdapter()
	, m_descSwapChain()
	, m_descDepthStencil()
	, m_cpDevice(nullptr)
	, m_cpImmContext(nullptr)
	, m_supportedMSAA()
	, m_cpSwapChain(nullptr)
	, m_cpSwapChainRTV(nullptr)
	, m_cpSwapChainDSV(nullptr)
	, m_vs{}
	, m_hs{}
	, m_ds{}
	, m_ps{}
	, m_il{}
	, m_rasterizerStates{}
	, m_samplerStates{}
	, m_depthStencilStates{}
	, m_skyboxSamplerState()
	, m_heightmapSamplerState()
{
}

GraphicDevice::~GraphicDevice()
{
}

void GraphicDevice::Init(void* pDesc)
{
	HRESULT hr;

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
		m_cpDevice.GetAddressOf(),
		&maxSupportedFeatureLevel,
		m_cpImmContext.GetAddressOf()
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"D3D11CreateDevice()", hr);

	if (maxSupportedFeatureLevel < D3D_FEATURE_LEVEL_11_0)
		Debug::ForceCrashWithMessageBox(L"Fail", L"Device does not support D3D11 feature level.");

	// �ϵ����� �����ϴ� ��Ƽ���ø� ī��Ʈ üũ
	// FEATURE_LEVEL_11_0 devices are required to support 4x MSAA for all render target formats,
	// and 8x MSAA for all render target formats except R32G32B32A32 formats.
	this->CreateSupportedMSAAQualityInfo();

	this->InitializeSwapChainAndDepthStencilBufferDesc();

	// ���� ü�� ����
	// First, Get IDXGIFactory instance
	// DirectX Graphics Infrastructure(DXGI)
	ComPtr<IDXGIDevice> cpDXGIDevice;
	ComPtr<IDXGIAdapter> cpDXGIAdapter;
	ComPtr<IDXGIFactory> cpDXGIFactory;

	// Use the IDXGIFactory instance that was used to create the device! (by COM queries)
	do
	{
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

		// Create a swap chain.
		hr = cpDXGIFactory->CreateSwapChain(m_cpDevice.Get(), &m_descSwapChain, m_cpSwapChain.GetAddressOf());
		if (FAILED(hr))
			break;

		// ���� ü���� ����ۿ� ���� ���� Ÿ�� �� �����
		ComPtr<ID3D11Texture2D> cpBackBuffer;
		hr = m_cpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(cpBackBuffer.GetAddressOf()));
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::GetBuffer()", hr);

		hr = m_cpDevice->CreateRenderTargetView(cpBackBuffer.Get(), nullptr, m_cpSwapChainRTV.GetAddressOf());
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateRenderTargetView()", hr);

		// ���� ü�ο� ���� �������� ����� ���� ���ٽ� �� �����
		ComPtr<ID3D11Texture2D> cpDepthStencilBuffer;
		hr = m_cpDevice->CreateTexture2D(&m_descDepthStencil, nullptr, cpDepthStencilBuffer.GetAddressOf());
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateTexture2D()", hr);

		hr = m_cpDevice->CreateDepthStencilView(cpDepthStencilBuffer.Get(), nullptr, m_cpSwapChainDSV.GetAddressOf());
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateDepthStencilView()", hr);
	} while (false);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Failed to create a swap chain.", hr);

	// Detect if newly created full-screen swap chain isn't actually full screen.
	// https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgifactory-createswapchain
	// Remark ����
	// ��üȭ������ ���� ü���� �����ϰ� Present�� ȣ���� �� ��Ÿ���� ����ġ �ʰ� ��üȭ���� ������ �� �ִ� ������ �ذ��ϴ� �ڵ�.
	if (m_descSwapChain.Windowed == FALSE)
	{
		BOOL isFullscreen;
		hr = this->GetFullscreenState(&isFullscreen);
		// If not full screen, enable full screen again.
		if (SUCCEEDED(hr) && isFullscreen == FALSE)
		{
			ShowWindow(Window::GetInstance().GetWindowHandle(), SW_MINIMIZE);
			ShowWindow(Window::GetInstance().GetWindowHandle(), SW_RESTORE);
			m_cpSwapChain->SetFullscreenState(TRUE, nullptr);
		}
	}

	// Disable ALT+ENTER window mode change
	hr = cpDXGIFactory->MakeWindowAssociation(Window::GetInstance().GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGIFactory::MakeWindowAssociation()", hr);

	// ���������������������������������������������������������� CREATE RASTERIZER STATES ����������������������������������������������������������
	this->CreateRasterizerStates();

	// �������������������������������������������������������� CREATE DEPTHSTENCIL STATES ��������������������������������������������������������
	this->CreateDepthStencilStates();

	// �������������������������������������������������������������� CREATE SAMPLER STATES ������������������������������������������������������������
	this->CreateSamplerStates();

	// �������������������������������������������������� CREATE SHADERS AND INPUT LAYOUTS ��������������������������������������������������
	this->CreateShaderAndInputLayout();
}

void GraphicDevice::Release()
{
	HRESULT hr;

	// ������������������������������������������������ RELEASE SHADERS AND INPUT LAYOUTS ��������������������������������������������������
	this->ReleaseShaderAndInputLayout();

	// ���������������������������������������������������������� RELEASE SAMPLER STATES ������������������������������������������������������������
	this->ReleaseSamplerStates();

	// ������������������������������������������������������ RELEASE DEPTHSTENCIL STATES ��������������������������������������������������������
	this->ReleaseDepthStencilStates();

	// ������������������������������������������������������ RELEASE RASTERIZER STATES ������������������������������������������������������������
	this->ReleaseRasterizerStates();

	m_cpSwapChainRTV.Reset();
	m_cpSwapChainDSV.Reset();

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

	m_cpSwapChain.Reset();
	m_supportedMSAA.clear();
	m_cpImmContext.Reset();
	m_cpDevice.Reset();
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

void GraphicDevice::CreateShaderAndInputLayout()
{
	std::vector<byte> shaderByteCode;

	// 1. POSITION
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_VS_PATH[static_cast<size_t>(VERTEX_SHADER_TYPE::SKYBOX_TRANSFORM)]);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::SKYBOX_TRANSFORM)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_VS_PATH[static_cast<size_t>(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_P)]);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_P)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());
	// ���� Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION)].Init(
		m_cpDevice.Get(),
		VertexFormat::Position::GetInputElementDescriptor(),
		VertexFormat::Position::GetInputElementCount(),
		shaderByteCode.data(),
		shaderByteCode.size()
	);

	// 2. POSITION, COLOR
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_VS_PATH[static_cast<size_t>(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PC)]);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PC)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());
	// ���� Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_COLOR)].Init(
		m_cpDevice.Get(),
		VertexFormat::PositionColor::GetInputElementDescriptor(),
		VertexFormat::PositionColor::GetInputElementCount(),
		shaderByteCode.data(),
		shaderByteCode.size()
	);

	// 3. POSITION, NORMAL
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_VS_PATH[static_cast<size_t>(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PN)]);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PN)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());
	// ���� Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_NORMAL)].Init(
		m_cpDevice.Get(),
		VertexFormat::PositionNormal::GetInputElementDescriptor(),
		VertexFormat::PositionNormal::GetInputElementCount(),
		shaderByteCode.data(),
		shaderByteCode.size()
	);

	// 4. POSITION, TEXCOORD
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_VS_PATH[static_cast<size_t>(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PT)]);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PT)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_VS_PATH[static_cast<size_t>(VERTEX_SHADER_TYPE::RENDER_RESULT_MERGE)]);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::RENDER_RESULT_MERGE)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());
	// ���� Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_TEXCOORD)].Init(
		m_cpDevice.Get(),
		VertexFormat::PositionTexCoord::GetInputElementDescriptor(),
		VertexFormat::PositionTexCoord::GetInputElementCount(),
		shaderByteCode.data(),
		shaderByteCode.size()
	);

	// 5. POSITION, NORMAL, TEXCOORD
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_VS_PATH[static_cast<size_t>(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PNT)]);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PNT)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());
	// ���� Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD)].Init(
		m_cpDevice.Get(),
		VertexFormat::PositionNormalTexCoord::GetInputElementDescriptor(),
		VertexFormat::PositionNormalTexCoord::GetInputElementCount(),
		shaderByteCode.data(),
		shaderByteCode.size()
	);

	// 6. POSITION, TEXCOORD0, TEXCOORD1
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_VS_PATH[static_cast<size_t>(VERTEX_SHADER_TYPE::TERRAIN_TRANSFORM)]);
	m_vs[static_cast<size_t>(VERTEX_SHADER_TYPE::TERRAIN_TRANSFORM)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());
	// ���� Create compatible Input Layout
	m_il[static_cast<size_t>(VERTEX_FORMAT_TYPE::TERRAIN_CONTROL_POINT)].Init(
		m_cpDevice.Get(),
		VertexFormat::TerrainControlPoint::GetInputElementDescriptor(),
		VertexFormat::TerrainControlPoint::GetInputElementCount(),
		shaderByteCode.data(),
		shaderByteCode.size()
	);

	// 1. HSTerrainRendering
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_HS_PATH[static_cast<size_t>(HULL_SHADER_TYPE::TERRAIN_RENDERING)]);
	m_hs[static_cast<size_t>(HULL_SHADER_TYPE::TERRAIN_RENDERING)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	// 1. DSTerrainRendering
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_DS_PATH[static_cast<size_t>(DOMAIN_SHADER_TYPE::TERRAIN_RENDERING)]);
	m_ds[static_cast<size_t>(DOMAIN_SHADER_TYPE::TERRAIN_RENDERING)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	// 1. PSSkyboxColoring
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_PS_PATH[static_cast<size_t>(PIXEL_SHADER_TYPE::SKYBOX_COLORING)]);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::SKYBOX_COLORING)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	// 2. PSTerrainColoring
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_PS_PATH[static_cast<size_t>(PIXEL_SHADER_TYPE::TERRAIN_COLORING)]);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::TERRAIN_COLORING)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	// 3. PSStandardColoringP
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_PS_PATH[static_cast<size_t>(PIXEL_SHADER_TYPE::STANDARD_COLORING_P)]);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::STANDARD_COLORING_P)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	// 4. PSStandardColoringPC
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_PS_PATH[static_cast<size_t>(PIXEL_SHADER_TYPE::STANDARD_COLORING_PC)]);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::STANDARD_COLORING_PC)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	// 5. PSStandardColoringPN
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_PS_PATH[static_cast<size_t>(PIXEL_SHADER_TYPE::STANDARD_COLORING_PN)]);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::STANDARD_COLORING_PN)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	// 6. PSStandardColoringPT
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_PS_PATH[static_cast<size_t>(PIXEL_SHADER_TYPE::STANDARD_COLORING_PT)]);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::STANDARD_COLORING_PT)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	// 7. PSRenderResultMerge (Not implemented)
	// shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_PS_PATH[static_cast<size_t>(PIXEL_SHADER_TYPE::RENDER_RESULT_MERGE)]);
	// m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::RENDER_RESULT_MERGE)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	// 8. PSMSRenderResultMerge
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_PS_PATH[static_cast<size_t>(PIXEL_SHADER_TYPE::MSRENDER_RESULT_MERGE)]);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::MSRENDER_RESULT_MERGE)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());

	// 9. PSStandardColoringPNT
	shaderByteCode = GraphicDevice::LoadShaderByteCode(ENGINEBIN_CSO_PS_PATH[static_cast<size_t>(PIXEL_SHADER_TYPE::STANDARD_COLORING_PNT)]);
	m_ps[static_cast<size_t>(PIXEL_SHADER_TYPE::STANDARD_COLORING_PNT)].Init(m_cpDevice.Get(), shaderByteCode.data(), shaderByteCode.size());
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
	m_rasterizerStates[static_cast<size_t>(RASTERIZER_FILL_MODE::WIREFRAME)][static_cast<size_t>(RASTERIZER_CULL_MODE::NONE)].Init(
		pDevice, &descRS
	);

	descRS.FillMode = D3D11_FILL_WIREFRAME;
	descRS.CullMode = D3D11_CULL_FRONT;
	m_rasterizerStates[static_cast<size_t>(RASTERIZER_FILL_MODE::WIREFRAME)][static_cast<size_t>(RASTERIZER_CULL_MODE::FRONT)].Init(
		pDevice, &descRS
	);

	descRS.FillMode = D3D11_FILL_WIREFRAME;
	descRS.CullMode = D3D11_CULL_BACK;
	m_rasterizerStates[static_cast<size_t>(RASTERIZER_FILL_MODE::WIREFRAME)][static_cast<size_t>(RASTERIZER_CULL_MODE::BACK)].Init(
		pDevice, &descRS
	);

	descRS.FillMode = D3D11_FILL_SOLID;
	descRS.CullMode = D3D11_CULL_NONE;
	m_rasterizerStates[static_cast<size_t>(RASTERIZER_FILL_MODE::SOLID)][static_cast<size_t>(RASTERIZER_CULL_MODE::NONE)].Init(
		pDevice, &descRS
	);

	descRS.FillMode = D3D11_FILL_SOLID;
	descRS.CullMode = D3D11_CULL_FRONT;
	m_rasterizerStates[static_cast<size_t>(RASTERIZER_FILL_MODE::SOLID)][static_cast<size_t>(RASTERIZER_CULL_MODE::FRONT)].Init(
		pDevice, &descRS
	);

	descRS.FillMode = D3D11_FILL_SOLID;
	descRS.CullMode = D3D11_CULL_BACK;
	m_rasterizerStates[static_cast<size_t>(RASTERIZER_FILL_MODE::SOLID)][static_cast<size_t>(RASTERIZER_CULL_MODE::BACK)].Init(
		pDevice, &descRS
	);
}

void GraphicDevice::ReleaseRasterizerStates()
{
	for (size_t i = 0; i < static_cast<size_t>(RASTERIZER_FILL_MODE::COUNT); ++i)
		for (size_t j = 0; j < static_cast<size_t>(RASTERIZER_CULL_MODE::COUNT); ++j)
			m_rasterizerStates[i][j].Release();
}

void GraphicDevice::CreateSamplerStates()
{
	ID3D11Device* pDevice = m_cpDevice.Get();

	D3D11_SAMPLER_DESC descSampler;

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_samplerStates[static_cast<size_t>(TEXTURE_FILTERING_OPTION::POINT)].Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; // �ּ��� (Bilinear)
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_samplerStates[static_cast<size_t>(TEXTURE_FILTERING_OPTION::BILINEAR)].Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // �Ｑ�� (Trilinear)
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_samplerStates[static_cast<size_t>(TEXTURE_FILTERING_OPTION::TRILINEAR)].Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_ANISOTROPIC;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MaxAnisotropy = 2;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_samplerStates[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_2X)].Init(pDevice, &descSampler);

	descSampler.MaxAnisotropy = 4;
	m_samplerStates[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X)].Init(pDevice, &descSampler);

	descSampler.MaxAnisotropy = 8;
	m_samplerStates[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_8X)].Init(pDevice, &descSampler);

	descSampler.MaxAnisotropy = 16;
	m_samplerStates[static_cast<size_t>(TEXTURE_FILTERING_OPTION::ANISOTROPIC_16X)].Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_skyboxSamplerState.Init(pDevice, &descSampler);

	ZeroMemory(&descSampler, sizeof(descSampler));
	descSampler.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;
	m_heightmapSamplerState.Init(pDevice, &descSampler);
}

void GraphicDevice::ReleaseSamplerStates()
{
	for (size_t i = 0; i < _countof(m_samplerStates); ++i)
		m_samplerStates[i].Release();

	m_skyboxSamplerState.Release();
	m_heightmapSamplerState.Release();
}

void GraphicDevice::CreateDepthStencilStates()
{
	ID3D11Device* pDevice = m_cpDevice.Get();

	D3D11_DEPTH_STENCIL_DESC descDepthStencil;
	ZeroMemory(&descDepthStencil, sizeof(descDepthStencil));

	// 1. �Ϲ� ��������
	descDepthStencil.DepthEnable = TRUE;
	descDepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.DepthFunc = D3D11_COMPARISON_LESS;
	descDepthStencil.StencilEnable = FALSE;	// �Ϲ� ��������
	m_depthStencilStates[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::STANDARD)].Init(pDevice, &descDepthStencil);

	// 2. ��ī�̹ڽ� ��������
	descDepthStencil.DepthEnable = TRUE;
	descDepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	// ��ī�̹ڽ� ������ �� �߿�
	descDepthStencil.StencilEnable = FALSE;
	m_depthStencilStates[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::SKYBOX)].Init(pDevice, &descDepthStencil);

	// 3. �ſ� ��������
	// ...
	// ...

	// 4. ī�޶� ���տ�
	ZeroMemory(&descDepthStencil, sizeof(descDepthStencil));
	descDepthStencil.DepthEnable = FALSE;
	descDepthStencil.StencilEnable = FALSE;
	m_depthStencilStates[static_cast<size_t>(DEPTH_STENCIL_STATE_TYPE::RENDER_RESULT_MERGE)].Init(pDevice, &descDepthStencil);
}

void GraphicDevice::ReleaseDepthStencilStates()
{
	for (size_t i = 0; i < _countof(m_depthStencilStates); ++i)
		m_depthStencilStates[i].Release();
}

std::vector<byte> GraphicDevice::LoadShaderByteCode(PCWSTR path)
{
	std::vector<byte> byteCode;
	FILE* csoFile = NULL;
	WCHAR filePath[MAX_PATH];
	filePath[0] = L'\0';

	HRESULT hr;

	hr = StringCbCopyW(filePath, sizeof(filePath), FileSystem::GetInstance().GetExeRelativePath());
	if (FAILED(hr))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to create full file path.\n%s", path);

	hr = StringCbCatW(filePath, sizeof(filePath), path);
	if (FAILED(hr))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to create full file path.\n%s", path);

	errno_t e = _wfopen_s(&csoFile, filePath, L"rb");
	if (e != 0)
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open compiled shader object.\n%s", path);

	fseek(csoFile, 0, SEEK_END);
	const size_t fileSize = static_cast<size_t>(ftell(csoFile));
	fseek(csoFile, 0, SEEK_SET);

	byteCode.resize(fileSize);

	const size_t bytesRead = static_cast<size_t>(fread_s(byteCode.data(), byteCode.size(), sizeof(byte), fileSize, csoFile));
	// Check error
	assert(bytesRead == fileSize);

	SafeCloseCRTFile(csoFile);

	return byteCode;
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
		hr = m_cpDevice->CheckMultisampleQualityLevels(BACKBUFFER_FORMAT, static_cast<UINT>(sc[i]), &quality);

		if (SUCCEEDED(hr) && quality != 0)
			m_supportedMSAA.push_back(std::make_pair(sc[i], quality - 1));
	}
}

void GraphicDevice::InitializeSwapChainAndDepthStencilBufferDesc()
{
	// Initialize SwapChain descriptor.
	m_descSwapChain.BufferDesc.Width = Window::GetInstance().GetWidth();
	m_descSwapChain.BufferDesc.Height = Window::GetInstance().GetHeight();
	m_descSwapChain.BufferDesc.RefreshRate.Numerator = 60;
	m_descSwapChain.BufferDesc.RefreshRate.Denominator = 1;
	m_descSwapChain.BufferDesc.Format = BACKBUFFER_FORMAT;
	m_descSwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_descSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// (�׽�Ʈ) 4X MSAA�� maximum quailty�� �������� ���
	// The default sampler mode, with no anti-aliasing, has a count of 1 and a quality level of 0.
	m_descSwapChain.SampleDesc.Count = static_cast<UINT>(MSAA_SAMPLE_COUNT::X4);
	m_descSwapChain.SampleDesc.Quality = this->GetMSAAMaximumQuality(MSAA_SAMPLE_COUNT::X4);	// Use max quality level

	m_descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_descSwapChain.BufferCount = 2;
	m_descSwapChain.OutputWindow = Window::GetInstance().GetWindowHandle();
	// m_descSwapChain.Windowed = m_fullscreenRequested ? FALSE : TRUE;
	m_descSwapChain.Windowed = TRUE;		// ���ʿ��� â ��� ����ü���� �����ϴ� ������ (MSDN ���� �����ؼ�..)
	m_descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	m_descSwapChain.Flags = SWAP_CHAIN_FLAG;

	// Depth/Stencil buffer (���� ũ��� �ݵ�� ���� ü���� �� ���ۿ� ��ġ�ؾ� ��)
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

void GraphicDevice::OnResize()
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

	// ���� ü���� ���� ���¸� �����´�.
	DXGI_SWAP_CHAIN_DESC descSwapChain;
	hr = m_cpSwapChain->GetDesc(&descSwapChain);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::GetDesc()", hr);

	const bool needResize =
		descSwapChain.BufferDesc.Width != Window::GetInstance().GetWidth() ||
		descSwapChain.BufferDesc.Height != Window::GetInstance().GetHeight();
	const bool isSwapChainFullscreen = descSwapChain.Windowed == FALSE;
	const bool needScreenModeChange = isSwapChainFullscreen != Window::GetInstance().IsFullscreen();

	if (needResize)
	{
		/*
		https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
		[Remarks] MSDN ����
		You can't resize a swap chain unless you release all outstanding references to its back buffers. <- �߿�

		You must release all of its 'direct' and 'indirect' references on the back buffers in order for ResizeBuffers to succeed.
		(���� ����)
		'Direct references' are held by the application after it calls AddRef on a resource.

		(���� ����)
		'Indirect references' are held by views to a resource, binding a view of the resource to a device context,
		a command list that used the resource, a command list that used a view to that resource,
		a command list that executed another command list that used the resource, and so on.
		*/

		// ���� ü�ο� ���� ���ҽ��� ��� ����
		m_cpSwapChainRTV.Reset();
		m_cpSwapChainDSV.Reset();

		// m_pSwapChain->ResizeTarget();
		hr = m_cpSwapChain->ResizeBuffers(
			0,							// Set this number to zero to preserve the existing number of buffers in the swap chain.
			Window::GetInstance().GetWidth(),
			Window::GetInstance().GetHeight(),
			DXGI_FORMAT_UNKNOWN,		// Set this value to DXGI_FORMAT_UNKNOWN to preserve the existing format of the back buffer.
			SWAP_CHAIN_FLAG
		);
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::ResizeBuffers()", hr);

		// ���� ü���� ����ۿ� ���� ���� Ÿ�� �� �����
		ComPtr<ID3D11Texture2D> cpBackBuffer;
		hr = m_cpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(cpBackBuffer.GetAddressOf()));
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::GetBuffer()", hr);

		hr = m_cpDevice->CreateRenderTargetView(cpBackBuffer.Get(), nullptr, m_cpSwapChainRTV.GetAddressOf());
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateRenderTargetView()", hr);

		// ���� ü�ο� ���� �������� ����� ���� ���ٽ� �� �����
		ComPtr<ID3D11Texture2D> cpDepthStencilBuffer;
		hr = m_cpDevice->CreateTexture2D(&m_descDepthStencil, nullptr, cpDepthStencilBuffer.GetAddressOf());
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateTexture2D()", hr);

		hr = m_cpDevice->CreateDepthStencilView(cpDepthStencilBuffer.Get(), nullptr, m_cpSwapChainDSV.GetAddressOf());
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"ID3D11Device::CreateDepthStencilView()", hr);
	}

	if (needScreenModeChange)
	{
		// ���÷��̰� �����ϴ� �ػ󵵿��߸� �Ѵ�. (����ڿ��� �����ϴ� ��� ���� �ʿ�)

		// MSDN
		// â ����� ���� ü���� �����ϰ� ���� ����ڰ� SetFullscreenState�� ���� ���� ü���� ��ü ȭ������ ������ �� �ֵ��� �ϴ� ���� �����ϴ�.
		// ��, ���� ü���� ��ü ȭ������ ������ �����ϱ� ���� DXGI_SWAP_CHAIN_DESC �� Windowed ����� FALSE�� �������� ���ʽÿ�.
		// �׷��� ���� ü���� ��ü ȭ������ �����ϴ� ��� ���� ����ڿ��� �����Ǵ� ���÷��� ��� ��ϵ� �����ؾ� �մϴ�.
		// �������� �ʴ� ���÷��� ���� ������ ���� ü���� ���÷��̰� �˰� ���ϰ� ���� ����ڰ� �ƹ��͵� �� �� ���� ���� �� �ֱ� �����Դϴ�. (�߿�!)
		// ���� ���� ����ڰ� ���÷��� ��带 ������ �� �ֵ��� ����� �� �ð� �ʰ� Ȯ�� ȭ���̳� ��Ÿ ��ü ��Ŀ������ ���� ���� �����ϴ�.

		const BOOL fullscreen = Window::GetInstance().IsFullscreen() ? TRUE : FALSE;
		hr = m_cpSwapChain->SetFullscreenState(fullscreen, nullptr);	// Windowed -> Fullscreen
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::SetFullscreenState()", hr);
	}

	// ���� ü�� ���� ���� (DXGI�� ���� ����ڳ� �ý��� ��û�� ���� ���� ü���� ǥ�� ���¸� ������ �� �ֽ��ϴ�)
	// �ý��� ��û�� ���� ����Ǿ��� �� ������ ���Ƿ� m_descSwapChain �����ϴ� ��� ���ٴ� �ٽ� GetDesc ȣ���ϴ� ������
	hr = m_cpSwapChain->GetDesc(&descSwapChain);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"IDXGISwapChain::GetDesc()", hr);
}

HRESULT GraphicDevice::GetFullscreenState(BOOL* pFullscreen)
{
	assert(pFullscreen != nullptr);

	ComPtr<IDXGIOutput> cpTarget;
	HRESULT hr = m_cpSwapChain->GetFullscreenState(pFullscreen, cpTarget.GetAddressOf());
	if (FAILED(hr))
		GlobalLog::GetInstance().GetSyncFileLogger().WriteFormat(
			L"GraphicDevice::GetFullscreenState() > IDXGISwapChain::GetFullscreenState() failed. HRESULT: 0x%x", hr
		);

	return hr;
}
