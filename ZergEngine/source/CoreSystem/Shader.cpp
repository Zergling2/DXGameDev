#include <ZergEngine\CoreSystem\Shader.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

static PCWSTR SHADER_CREATE_ERROR_MESSAGE = L"Failed to create a shader.";

VertexShader::~VertexShader()
{
	this->Release();
}

void VertexShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	Helper::SafeReleaseCom(m_pShader);

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

HullShader::~HullShader()
{
	this->Release();
}

void HullShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	Helper::SafeReleaseCom(m_pShader);

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

DomainShader::~DomainShader()
{
	this->Release();
}

void DomainShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	Helper::SafeReleaseCom(m_pShader);

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

PixelShader::~PixelShader()
{
	this->Release();
}

void PixelShader::Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	Helper::SafeReleaseCom(m_pShader);

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
