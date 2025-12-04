#pragma once

#include <ZergEngine\CoreSystem\ShaderResource\DX11Buffer.h>

namespace ze
{
	class IndexBuffer : public IDX11Buffer
	{
	public:
		virtual bool Init(ID3D11Device* pDevice, const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData) override;
	};
}
