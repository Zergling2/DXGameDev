#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class Texture2D;

	class ImageEffect : public IEffect
	{
	private:
		enum DirtyFlag : DWORD
		{
			PrimitiveTopology	= 1 << 0,
			InputLayout			= 1 << 1,
			Shader				= 1 << 2,
			ApplyCBUIRender		= 1 << 3,
			ApplyCBPer2DQuad	= 1 << 4,
			UpdateCBUIRender	= 1 << 5,
			UpdateCBPer2DQuad	= 1 << 6,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		ImageEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbUIRender()
			, m_cbPer2DQuad()
			, m_cbUIRenderCache()
			, m_cbPer2DQuadCache()
			, m_pTextureSRVArray{ nullptr }
		{
		}
		virtual ~ImageEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void XM_CALLCONV SetOrthoMatrix(FXMMATRIX m) noexcept;

		void SetSize(FLOAT width, FLOAT height) noexcept;
		void SetUVScale(FLOAT sx, FLOAT sy) noexcept;
		void SetUVOffset(FLOAT x, FLOAT y) noexcept;
		void SetHCSPosition(const XMFLOAT2& pos) noexcept;	// 화면 중앙을 원점으로 하는 NDC 공간으로 변환 직전의 2D 위치 설정

		void SetImageTexture(const Texture2D& image) noexcept;
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void OnUnbindFromDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPer2DQuadConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;

		void ClearTextureSRVArray() { m_pTextureSRVArray[0] = nullptr; }
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbUIRender> m_cbUIRender;
		ConstantBuffer<CbPer2DQuad> m_cbPer2DQuad;
		CbUIRender m_cbUIRenderCache;
		CbPer2DQuad m_cbPer2DQuadCache;

		ID3D11ShaderResourceView* m_pTextureSRVArray[1];
	};
}
