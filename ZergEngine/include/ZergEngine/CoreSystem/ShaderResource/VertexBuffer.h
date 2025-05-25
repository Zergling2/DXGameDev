#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class VertexBuffer
	{
	public:
		VertexBuffer()
			: m_pBuffer(nullptr)
		{
		}
		~VertexBuffer() { this->Release(); }

		void Init(ID3D11Device* pDevice, const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData);
		void Release();
		ID3D11Buffer* GetComInterface() { return m_pBuffer; }
	private:
		ID3D11Buffer* m_pBuffer;
	};
}