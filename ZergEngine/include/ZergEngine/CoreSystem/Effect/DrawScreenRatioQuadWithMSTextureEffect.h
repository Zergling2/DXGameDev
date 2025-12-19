#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class DrawScreenRatioQuadWithMSTextureEffect : public IEffect
	{
	private:
		enum DirtyFlag : DWORD
		{
			PrimitiveTopology		= 1 << 0,
			InputLayout				= 1 << 1,
			Shader					= 1 << 2,
			CBPerScreenRatioQuad	= 1 << 3,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		DrawScreenRatioQuadWithMSTextureEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerScreenRatioQuad()
			, m_cbPerScreenRatioQuadCache()
			, m_pTextureSRVArray{ nullptr }
		{
		}
		virtual ~DrawScreenRatioQuadWithMSTextureEffect() = default;

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

		ConstantBuffer<CbPerScreenRatioQuad> m_cbPerScreenRatioQuad;
		CbPerScreenRatioQuad m_cbPerScreenRatioQuadCache;

		// [0] RENDER_RESULT
		ID3D11ShaderResourceView* m_pTextureSRVArray[1];
	};
}
