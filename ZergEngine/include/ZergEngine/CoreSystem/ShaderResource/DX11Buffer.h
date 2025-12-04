#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class IDX11Buffer
	{
	public:
		IDX11Buffer()
			: m_cpBuffer()
		{
		}
		virtual ~IDX11Buffer() = default;

		virtual bool Init(ID3D11Device* pDevice, const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData);
		void Release();
		ID3D11Buffer* GetComInterface() const { return m_cpBuffer.Get(); }
	private:
		ComPtr<ID3D11Buffer> m_cpBuffer;
	};
}
