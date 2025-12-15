#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class BillboardEffect : public IEffect
	{
	private:
		enum DIRTY_FLAG : DWORD
		{
			PRIMITIVE_TOPOLOGY		= 1 << 0,
			INPUT_LAYOUT			= 1 << 1,
			SHADER					= 1 << 2,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		BillboardEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader{ nullptr, nullptr, nullptr }
			, m_cbPerBillboard()
			, m_cbPerBillboardCache()
		{
		}
		virtual ~BillboardEffect() = default;

		virtual void Init() override;
		virtual void Release() override;
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader[3];

		ConstantBuffer<CbPerBillboard> m_cbPerBillboard;
		CbPerBillboard m_cbPerBillboardCache;
	};
}
