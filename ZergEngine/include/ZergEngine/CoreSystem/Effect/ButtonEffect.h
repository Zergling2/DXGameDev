#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>
#include <string>

namespace ze
{
	class ButtonEffect : public IEffect
	{
	private:
		enum DIRTY_FLAG : DWORD
		{
			PRIMITIVE_TOPOLOGY				= 1 << 0,
			INPUT_LAYOUT					= 1 << 1,
			SHADER							= 1 << 2,
			CONSTANTBUFFER_PER_UI_RENDER	= 1 << 3,
			CONSTANTBUFFER_PER_BUTTON		= 1 << 4,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		ButtonEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerUIRender()
			, m_cbPerButton()
			, m_cbPerUIRenderCache()
			, m_cbPerButtonCache()
			, mText()
		{
		}
		virtual ~ButtonEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept;

		void XM_CALLCONV SetColor(FXMVECTOR color) noexcept;
		void SetPressed(bool pressed) noexcept;
		void XM_CALLCONV SetSize(FXMVECTOR size) noexcept;
		void XM_CALLCONV SetPreNDCPosition(FXMVECTOR position) noexcept;	// 화면 중앙을 원점으로 하는 NDC 공간으로 변환 직전의 2D 위치 설정
		void SetText(PCWSTR text) { mText = text; }
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerButtonConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbPerUIRender> m_cbPerUIRender;
		ConstantBuffer<CbPerButton> m_cbPerButton;
		CbPerUIRender m_cbPerUIRenderCache;
		CbPerButton m_cbPerButtonCache;
		std::wstring mText;
	};
}
