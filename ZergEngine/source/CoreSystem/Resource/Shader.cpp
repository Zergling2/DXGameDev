#include <ZergEngine\CoreSystem\Resource\Shader.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

void VertexShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	HRESULT hr;

	hr = pDevice->CreateVertexShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		m_cpShader.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"VertexShader::Init()", hr);
}

void HullShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	HRESULT hr;

	hr = pDevice->CreateHullShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		m_cpShader.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"HullShader::Init()", hr);
}

void DomainShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	HRESULT hr;

	hr = pDevice->CreateDomainShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		m_cpShader.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"DomainShader::Init()", hr);
}

void PixelShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	HRESULT hr;

	hr = pDevice->CreatePixelShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		m_cpShader.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"PixelShader::Init()", hr);
}
