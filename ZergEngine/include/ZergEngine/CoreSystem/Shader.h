#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class VertexShader
	{
	public:
		VertexShader() = default;
		~VertexShader() = default;

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		void Release();
		ID3D11VertexShader* Get() { return m_cpShader.Get(); }
	private:
		ComPtr<ID3D11VertexShader> m_cpShader;
	};

	class HullShader
	{
	public:
		HullShader() = default;
		~HullShader() = default;

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		void Release();
		ID3D11HullShader* Get() { return m_cpShader.Get(); }
	private:
		ComPtr<ID3D11HullShader> m_cpShader;
	};

	class DomainShader
	{
	public:
		DomainShader() = default;
		~DomainShader() = default;

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		void Release();
		ID3D11DomainShader* Get() { return m_cpShader.Get(); }
	private:
		ComPtr<ID3D11DomainShader> m_cpShader;
	};

	class PixelShader
	{
	public:
		PixelShader() = default;
		~PixelShader() = default;

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		void Release();
		ID3D11PixelShader* Get() { return m_cpShader.Get(); }
	private:
		ComPtr<ID3D11PixelShader> m_cpShader;
	};
}
