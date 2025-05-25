#include <ZergEngine\CoreSystem\Shader.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

static PCWSTR SHADER_CREATE_ERROR_MESSAGE = L"Failed to create a shader.";

void VertexShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	this->Release();

	HRESULT hr;

	hr = pDevice->CreateVertexShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		&m_pShader
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(SHADER_CREATE_ERROR_MESSAGE, hr);
}

void VertexShader::Release()
{
	Helper::SafeReleaseCom(m_pShader);
}

void HullShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	this->Release();

	HRESULT hr;

	hr = pDevice->CreateHullShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		&m_pShader
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(SHADER_CREATE_ERROR_MESSAGE, hr);
}

void HullShader::Release()
{
	Helper::SafeReleaseCom(m_pShader);
}

void DomainShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	this->Release();

	HRESULT hr;

	hr = pDevice->CreateDomainShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		&m_pShader
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(SHADER_CREATE_ERROR_MESSAGE, hr);
}

void DomainShader::Release()
{
	Helper::SafeReleaseCom(m_pShader);
}

void PixelShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	this->Release();

	HRESULT hr;

	hr = pDevice->CreatePixelShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		&m_pShader
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(SHADER_CREATE_ERROR_MESSAGE, hr);
}

void PixelShader::Release()
{
	Helper::SafeReleaseCom(m_pShader);
}
