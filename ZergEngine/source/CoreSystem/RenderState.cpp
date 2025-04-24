#include <ZergEngine\CoreSystem\RenderState.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\Common\EngineHelper.h>

using namespace ze;

RasterizerState::~RasterizerState()
{
	this->Release();
}

void RasterizerState::Init(ID3D11Device* pDevice, const D3D11_RASTERIZER_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateRasterizerState(pDesc, &m_pRasterizerState);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"RasterizerState::Init()", hr);
}

void RasterizerState::Release()
{
	Helper::SafeReleaseCom(m_pRasterizerState);
}

SamplerState::~SamplerState()
{
	this->Release();
}

void SamplerState::Init(ID3D11Device* pDevice, const D3D11_SAMPLER_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateSamplerState(pDesc, &m_pSamplerState);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"SamplerState::Init()", hr);
}

void SamplerState::Release()
{
	Helper::SafeReleaseCom(m_pSamplerState);
}

DepthStencilState::~DepthStencilState()
{
	this->Release();
}

void DepthStencilState::Init(ID3D11Device* pDevice, const D3D11_DEPTH_STENCIL_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateDepthStencilState(pDesc, &m_pDepthStencilState);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"DepthStencilState::Init()", hr);
}

void DepthStencilState::Release()
{
	Helper::SafeReleaseCom(m_pDepthStencilState);
}

BlendState::~BlendState()
{
	this->Release();
}

void BlendState::Init(ID3D11Device* pDevice, const D3D11_BLEND_DESC* pDesc)
{
	HRESULT hr;

	hr = pDevice->CreateBlendState(pDesc, &m_pBlendState);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"BlendState::Init()", hr);
}

void BlendState::Release()
{
	Helper::SafeReleaseCom(m_pBlendState);
}
