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
			ApplyCBUIRender		= 1 << 3,
			ApplyCBPerCheckbox	= 1 << 4,
			UpdateCBUIRender	= 1 << 5,
			UpdateCBPerCheckbox	= 1 << 6,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		CheckboxEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbUIRender()
			, m_cbPerCheckbox()
			, m_cbUIRenderCache()
			, m_cbPerCheckboxCache()
		{
		}
		virtual ~CheckboxEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void XM_CALLCONV SetOrthoMatrix(FXMMATRIX m) noexcept;

		void XM_CALLCONV SetBoxColor(FXMVECTOR color) noexcept;
		void XM_CALLCONV SetCheckColor(FXMVECTOR color) noexcept;
		void SetSize(FLOAT width, FLOAT height) noexcept;
		void SetHCSPosition(const XMFLOAT2& pos) noexcept;	// 화면 중앙을 원점으로 하는 NDC 공간으로 변환 직전의 2D 위치 설정
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void OnUnbindFromDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerCheckboxConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbUIRender> m_cbUIRender;
		ConstantBuffer<CbPerCheckbox> m_cbPerCheckbox;
		CbUIRender m_cbUIRenderCache;
		CbPerCheckbox m_cbPerCheckboxCache;
	};
}
