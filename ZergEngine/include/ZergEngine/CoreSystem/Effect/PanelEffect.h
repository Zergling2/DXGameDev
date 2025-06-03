#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class PanelEffect : public IEffect
	{
	private:
		enum DIRTY_FLAG : DWORD
		{
			PRIMITIVE_TOPOLOGY				= 1 << 0,
			INPUT_LAYOUT					= 1 << 1,
			SHADER							= 1 << 2,
			CONSTANTBUFFER_PER_UI_RENDER	= 1 << 3,
			CONSTANTBUFFER_PER_PANEL		= 1 << 4,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		PanelEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerUIRender()
			, m_cbPerPanel()
			, m_cbPerUIRenderCache()
			, m_cbPerPanelCache()
		{
		}
		virtual ~PanelEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept;

		void XM_CALLCONV SetColor(FXMVECTOR color) noexcept;
		void XM_CALLCONV SetSize(FXMVECTOR size) noexcept;
		void XM_CALLCONV SetPreNDCPosition(FXMVECTOR position) noexcept;	// ȭ�� �߾��� �������� �ϴ� NDC �������� ��ȯ ������ 2D ��ġ ����
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerPanelConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbPerUIRender> m_cbPerUIRender;
		ConstantBuffer<CbPerPCQuad> m_cbPerPanel;
		CbPerUIRender m_cbPerUIRenderCache;
		CbPerPCQuad m_cbPerPanelCache;
	};
}
