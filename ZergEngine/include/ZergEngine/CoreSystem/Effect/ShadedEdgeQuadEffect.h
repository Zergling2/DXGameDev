#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class Texture2D;

	class ShadedEdgeQuadEffect : public IEffect
	{
	private:
		enum DirtyFlag : DWORD
		{
			PrimitiveTopology	= 1 << 0,
			InputLayout			= 1 << 1,
			Shader				= 1 << 2,
			ApplyCB2DRender		= 1 << 3,
			ApplyCBPer2DQuad	= 1 << 4,
			UpdateCB2DRender	= 1 << 5,
			UpdateCBPer2DQuad	= 1 << 6,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		ShadedEdgeQuadEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cb2DRender()
			, m_cbPer2DQuad()
			, m_cb2DRenderCache()
			, m_cbPer2DQuadCache()
		{
		}
		virtual ~ShadedEdgeQuadEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept;

		void SetSize(FLOAT width, FLOAT height) noexcept;
		void SetViewSpacePosition(const XMFLOAT2& pos) noexcept;	// 화면 중앙을 원점으로 하는 NDC 공간으로 변환 직전의 2D 위치 설정

		void XM_CALLCONV SetColor(FXMVECTOR color) noexcept;
		void SetColorWeight(FLOAT lt, FLOAT rb) noexcept;
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void OnUnbindFromDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void Apply2DRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPer2DQuadConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<Cb2DRender> m_cb2DRender;
		ConstantBuffer<CbPer2DQuad> m_cbPer2DQuad;
		Cb2DRender m_cb2DRenderCache;
		CbPer2DQuad m_cbPer2DQuadCache;
	};
}
