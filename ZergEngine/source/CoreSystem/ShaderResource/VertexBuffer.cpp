#include <ZergEngine\CoreSystem\ShaderResource\VertexBuffer.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

bool VertexBuffer::Init(ID3D11Device* pDevice, const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData)
{
	if (pDesc->BindFlags != D3D11_BIND_VERTEX_BUFFER)
		Debug::ForceCrashWithMessageBox(L"Debug", L"Invalid bind flag passed.");

	return IDX11Buffer::Init(pDevice, pDesc, pInitialData);
}
