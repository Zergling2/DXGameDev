#include <ZergEngine\CoreSystem\RenderState.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

void RasterizerState::Init(ID3D11Device* pDevice, const D3D11_RASTERIZER_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateRasterizerState(pDesc, m_cpState.GetAddressOf());

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"RasterizerState::Init()", hr);
}

void RasterizerState::Release()
{
	m_cpState.Reset();
}

void SamplerState::Init(ID3D11Device* pDevice, const D3D11_SAMPLER_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateSamplerState(pDesc, m_cpState.GetAddressOf());

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"SamplerState::Init()", hr);
}

void SamplerState::Release()
{
	m_cpState.Reset();
}

void DepthStencilState::Init(ID3D11Device* pDevice, const D3D11_DEPTH_STENCIL_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateDepthStencilState(pDesc, m_cpState.GetAddressOf());

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"DepthStencilState::Init()", hr);
}

void DepthStencilState::Release()
{
	m_cpState.Reset();
}

void BlendState::Init(ID3D11Device* pDevice, const D3D11_BLEND_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateBlendState(pDesc, m_cpState.GetAddressOf());

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"BlendState::Init()", hr);
}

void BlendState::Release()
{
	m_cpState.Reset();
}
