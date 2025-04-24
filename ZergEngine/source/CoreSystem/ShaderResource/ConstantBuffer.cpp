#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

IConstantBuffer::~IConstantBuffer()
{
	Helper::SafeReleaseCom(m_pConstantBuffer);
}

void IConstantBuffer::Release()
{
	Helper::SafeReleaseCom(m_pConstantBuffer);
}

void IConstantBuffer::InitImpl(ID3D11Device* pDevice, size_t bufferSize) noexcept
{
	D3D11_BUFFER_DESC descConstantBuffer;
	ZeroMemory(&descConstantBuffer, sizeof(descConstantBuffer));

	descConstantBuffer.ByteWidth = static_cast<UINT>(bufferSize);
	descConstantBuffer.Usage = D3D11_USAGE_DYNAMIC;
	descConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	descConstantBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	descConstantBuffer.MiscFlags = 0;
	descConstantBuffer.StructureByteStride = 0;

	HRESULT hr = pDevice->CreateBuffer(&descConstantBuffer, nullptr, &m_pConstantBuffer);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Failed to create a constant buffer.", hr);
}

void IConstantBuffer::UpdateImpl(ID3D11DeviceContext* pDeviceContext, const void* pData, size_t size) noexcept
{
	assert(m_pConstantBuffer != nullptr);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	hr = pDeviceContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Failed to map constant buffer.", hr);

	memcpy(mapped.pData, pData, size);
	pDeviceContext->Unmap(m_pConstantBuffer, 0);
}
