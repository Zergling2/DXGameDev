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
			PrimitiveTopology			= 1 << 0,
			InputLayout					= 1 << 1,
			Shader						= 1 << 2,
			ApplyCBUIRender				= 1 << 3,
			ApplyCBPerShadedEdgeQuad	= 1 << 4,
			UpdateCBUIRender			= 1 << 5,
			UpdateCBPerShadedEdgeQuad	= 1 << 6,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		ShadedEdgeQuadEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbUIRender()
			, m_cbPerShadedEdgeQuad()
			, m_cbUIRenderCache()
			, m_cbPerShadedEdgeQuadCache()
		{
		}
		virtual ~ShadedEdgeQuadEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void XM_CALLCONV SetOrthoMatrix(FXMMATRIX m) noexcept;

		void SetSize(FLOAT width, FLOAT height) noexcept;
		void SetHCSPosition(const XMFLOAT2& pos) noexcept;	// 화면 중앙을 원점으로 하는 NDC 공간으로 변환 직전의 2D 위치 설정

		void XM_CALLCONV SetColor(FXMVECTOR color) noexcept;
		void SetColorWeight(FLOAT lt, FLOAT rb) noexcept;
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void OnUnbindFromDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerShadedEdgeQuadConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbUIRender> m_cbUIRender;
		ConstantBuffer<CbPerShadedEdgeQuad> m_cbPerShadedEdgeQuad;
		CbUIRender m_cbUIRenderCache;
		CbPerShadedEdgeQuad m_cbPerShadedEdgeQuadCache;
	};
}
