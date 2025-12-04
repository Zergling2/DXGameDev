#include <ZergEngine\CoreSystem\ShaderResource\DX11Buffer.h>
#include <ZergEngine\CoreSystem\Runtime.h>

using namespace ze;

bool IDX11Buffer::Init(ID3D11Device* pDevice, const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData)
{
	auto& sfl = Runtime::GetInstance()->GetSyncFileLogger();

	this->Release();

	HRESULT hr;

	assert(m_cpBuffer == nullptr);
	hr = pDevice->CreateBuffer(pDesc, pInitialData, m_cpBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID3D11Device::CreateBuffer()", hr);
		return false;
	}

	return true;
}

void IDX11Buffer::Release()
{
	m_cpBuffer.Reset();
}
