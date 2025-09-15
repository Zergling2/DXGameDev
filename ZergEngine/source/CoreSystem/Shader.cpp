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
		m_cpShader.GetAddressOf()
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(SHADER_CREATE_ERROR_MESSAGE, hr);
}

void VertexShader::Release()
{
	m_cpShader.Reset();
}

void HullShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	this->Release();

	HRESULT hr;

	hr = pDevice->CreateHullShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		m_cpShader.GetAddressOf()
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(SHADER_CREATE_ERROR_MESSAGE, hr);
}

void HullShader::Release()
{
	m_cpShader.Reset();
}

void DomainShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	this->Release();

	HRESULT hr;

	hr = pDevice->CreateDomainShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		m_cpShader.GetAddressOf()
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(SHADER_CREATE_ERROR_MESSAGE, hr);
}

void DomainShader::Release()
{
	m_cpShader.Reset();
}

void PixelShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	this->Release();

	HRESULT hr;

	hr = pDevice->CreatePixelShader(
		pShaderByteCode,
		shaderByteCodeSize,
		nullptr,
		m_cpShader.GetAddressOf()
	);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(SHADER_CREATE_ERROR_MESSAGE, hr);
}

void PixelShader::Release()
{
	m_cpShader.Reset();
}
