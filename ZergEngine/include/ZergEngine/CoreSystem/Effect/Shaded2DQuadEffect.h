#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>
#include <string>

namespace ze
{
	class Shaded2DQuadEffect : public IEffect
	{
	private:
		enum DirtyFlag : DWORD
		{
			PrimitiveTopology		= 1 << 0,
			InputLayout				= 1 << 1,
			Shader					= 1 << 2,
			CBPerUIRender			= 1 << 3,
			CBPerShaded2DQuad		= 1 << 4,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		Shaded2DQuadEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerUIRender()
			, m_cbPerShaded2DQuad()
			, m_cbPerUIRenderCache()
			, m_cbPerShaded2DQuadCache()
			, m_text()
		{
		}
		virtual ~Shaded2DQuadEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept;

		void XM_CALLCONV SetColor(FXMVECTOR color) noexcept;
		void SetShadeWeightIndex(uint32_t index) noexcept;
		void SetSize(FLOAT width, FLOAT height) noexcept;
		void SetHCSPosition(const XMFLOAT2& pos) noexcept;	// 화면 중앙을 원점으로 하는 NDC 공간으로 변환 직전의 2D 위치 설정
		void SetText(PCWSTR text) { m_text = text; }
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerShaded2DQuadConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbPerUIRender> m_cbPerUIRender;
		ConstantBuffer<CbPerShaded2DQuad> m_cbPerShaded2DQuad;
		CbPerUIRender m_cbPerUIRenderCache;
		CbPerShaded2DQuad m_cbPerShaded2DQuadCache;
		std::wstring m_text;
	};
}
