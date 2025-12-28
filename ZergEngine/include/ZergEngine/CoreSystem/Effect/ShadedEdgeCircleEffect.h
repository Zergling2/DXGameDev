#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class Texture2D;

	class ShadedEdgeCircleEffect : public IEffect
	{
	private:
		enum DirtyFlag : DWORD
		{
			PrimitiveTopology				= 1 << 0,
			InputLayout						= 1 << 1,
			Shader							= 1 << 2,
			ApplyCB2DRender = 1 << 3,
			ApplyCBPerShadedEdgeCircle = 1 << 4,
			UpdateCB2DRender = 1 << 5,
			UpdateCBPerShadedEdgeCircle = 1 << 6,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		ShadedEdgeCircleEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cb2DRender()
			, m_cbPerShadedEdgeCircle()
			, m_cb2DRenderCache()
			, m_cbPerShadedEdgeCircleCache()
		{
		}
		virtual ~ShadedEdgeCircleEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept;

		void SetRadius(FLOAT radius) noexcept;
		void SetHCSPosition(const XMFLOAT2& pos) noexcept;	// 화면 중앙을 원점으로 하는 NDC 공간으로 변환 직전의 2D 위치 설정

		void XM_CALLCONV SetColor(FXMVECTOR color) noexcept;
		void SetColorWeight(FLOAT lt, FLOAT rb) noexcept;
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void OnUnbindFromDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void Apply2DRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerShadedEdgeCircleConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<Cb2DRender> m_cb2DRender;
		ConstantBuffer<CbPerShadedEdgeCircle> m_cbPerShadedEdgeCircle;
		Cb2DRender m_cb2DRenderCache;
		CbPerShadedEdgeCircle m_cbPerShadedEdgeCircleCache;
	};
}
