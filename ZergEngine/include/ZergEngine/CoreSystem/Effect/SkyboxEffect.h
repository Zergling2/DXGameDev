#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class Camera;

	class SkyboxEffect : public IEffect
	{
	private:
		enum DIRTY_FLAG : DWORD
		{
			PRIMITIVE_TOPOLOGY			= 1 << 0,
			INPUT_LAYOUT				= 1 << 1,
			SHADER						= 1 << 2,
			CONSTANTBUFFER_PER_CAMERA	= 1 << 3,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		SkyboxEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerCamera()
			, m_cbPerCameraCache()
			, m_pTextureSRVArray{ nullptr }
		{
		}
		virtual ~SkyboxEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		// View Matrix, Projection Matrix가 업데이트 된 상태로 전달되어야 함. (=> 엔진 런타임에서 자동으로 처리중)
		void SetCamera(const Camera* pCamera) noexcept;

		void SetSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV) { m_pTextureSRVArray[0] = pSkyboxCubeMapSRV; }
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;

		void ClearTextureSRVArray();
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbPerCamera> m_cbPerCamera;
		CbPerCamera m_cbPerCameraCache;

		// [0] SKYBOX_CUBEMAP
		ID3D11ShaderResourceView* m_pTextureSRVArray[1];
	};
}
