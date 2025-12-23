#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>
#include <string>

namespace ze
{
	class CheckboxEffect : public IEffect
	{
	private:
		enum DirtyFlag : DWORD
		{
			PrimitiveTopology	= 1 << 0,
			InputLayout			= 1 << 1,
			Shader				= 1 << 2,
			CBPerUIRender		= 1 << 3,
			CBPerCheckbox		= 1 << 4,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		CheckboxEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerUIRender()
			, m_cbPerCheckbox()
			, m_cbPerUIRenderCache()
			, m_cbPerCheckboxCache()
		{
		}
		virtual ~CheckboxEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept;

		void XM_CALLCONV SetBoxColor(FXMVECTOR color) noexcept;
		void XM_CALLCONV SetCheckColor(FXMVECTOR color) noexcept;
		void SetSize(FLOAT width, FLOAT height) noexcept;
		void SetHCSPosition(const XMFLOAT2& pos) noexcept;	// 화면 중앙을 원점으로 하는 NDC 공간으로 변환 직전의 2D 위치 설정
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerCheckboxConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbPerUIRender> m_cbPerUIRender;
		ConstantBuffer<CbPerCheckbox> m_cbPerCheckbox;
		CbPerUIRender m_cbPerUIRenderCache;
		CbPerCheckbox m_cbPerCheckboxCache;
	};
}
