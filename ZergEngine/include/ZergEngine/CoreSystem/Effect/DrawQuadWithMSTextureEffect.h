#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class DrawQuadWithMSTextureEffect : public IEffect
	{
	private:
		enum DIRTY_FLAG : DWORD
		{
			PRIMITIVE_TOPOLOGY					= 1 << 0,
			INPUT_LAYOUT						= 1 << 1,
			SHADER								= 1 << 2,
			CONSTANTBUFFER_PER_DRAW_QUAD		= 1 << 3,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		DrawQuadWithMSTextureEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerDrawQuad()
			, m_cbPerDrawQuadCache()
			, m_pTextureSRVArray{ nullptr }
		{
		}
		virtual ~DrawQuadWithMSTextureEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void SetQuadParameters(float width, float height, float topLeftX, float topLeftY) noexcept;
		void SetTexture(ID3D11ShaderResourceView* pTextureSRV);
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerDrawQuadConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;

		void ClearTextureSRVArray();
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbPerDrawQuad> m_cbPerDrawQuad;
		CbPerDrawQuad m_cbPerDrawQuadCache;

		// [0] RENDER_RESULT
		ID3D11ShaderResourceView* m_pTextureSRVArray[1];
	};
}
