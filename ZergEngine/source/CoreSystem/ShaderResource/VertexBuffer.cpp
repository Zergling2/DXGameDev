#include <ZergEngine\CoreSystem\ShaderResource\VertexBuffer.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

void VertexBuffer::Init(ID3D11Device* pDevice, const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData)
{
	this->Release();

	HRESULT hr;

	assert(m_cpBuffer == nullptr);
	hr = pDevice->CreateBuffer(pDesc, pInitialData, m_cpBuffer.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateBuffer()", hr);
}

void VertexBuffer::Release()
{
	m_cpBuffer.Reset();
}
