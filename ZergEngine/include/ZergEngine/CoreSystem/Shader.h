#pragma once

#include <ZergEngine\Common\EngineHelper.h>

namespace ze
{
	class VertexShader
	{
	public:
		VertexShader()
			: m_pShader(nullptr)
		{
		}
		~VertexShader();

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		void Release();
		ID3D11VertexShader* GetComInterface() { return m_pShader; }
	private:
		ID3D11VertexShader* m_pShader;
	};

	class HullShader
	{
	public:
		HullShader()
			: m_pShader(nullptr)
		{
		}
		~HullShader();

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		void Release();
		ID3D11HullShader* GetComInterface() { return m_pShader; }
	private:
		ID3D11HullShader* m_pShader;
	};

	class DomainShader
	{
	public:
		DomainShader()
			: m_pShader(nullptr)
		{
		}
		~DomainShader();

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		void Release();
		ID3D11DomainShader* GetComInterface() { return m_pShader; }
	private:
		ID3D11DomainShader* m_pShader;
	};

	class PixelShader
	{
	public:
		PixelShader()
			: m_pShader(nullptr)
		{
		}
		~PixelShader();

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		void Release();
		ID3D11PixelShader* GetComInterface() { return m_pShader; }
	private:
		ID3D11PixelShader* m_pShader;
	};
}
