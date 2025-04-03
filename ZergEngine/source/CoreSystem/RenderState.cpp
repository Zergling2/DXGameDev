#include <ZergEngine\CoreSystem\RenderState.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

void RasterizerState::Init(ID3D11Device* pDevice, const D3D11_RASTERIZER_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateRasterizerState(pDesc, m_cpInterface.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"RasterizerState::Init()", hr);
}

void SamplerState::Init(ID3D11Device* pDevice, const D3D11_SAMPLER_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateSamplerState(pDesc, m_cpInterface.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"SamplerState::Init()", hr);
}

void DepthStencilState::Init(ID3D11Device* pDevice, const D3D11_DEPTH_STENCIL_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateDepthStencilState(pDesc, m_cpInterface.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"DepthStencilState::Init()", hr);
}

void BlendState::Init(ID3D11Device* pDevice, const D3D11_BLEND_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateBlendState(pDesc, m_cpInterface.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"BlendState::Init()", hr);
}
