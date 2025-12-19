#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class Camera;

	class BasicEffectP : public IEffect
	{
	private:
		enum DirtyFlag : DWORD
		{
			PrimitiveTopology			= 1 << 0,
			InputLayout				= 1 << 1,
			Shader						= 1 << 2,
			CBPerCamera	= 1 << 3,
			CBPerMesh		= 1 << 4,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		BasicEffectP() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerCamera()
			, m_cbPerMesh()
			, m_cbPerCameraCache()
			, m_cbPerMeshCache()
		{
		}
		virtual ~BasicEffectP() = default;

		virtual void Init() override;
		virtual void Release() override;

		// View Matrix, Projection Matrix가 업데이트 된 상태로 전달되어야 함. (=> 엔진 런타임에서 자동으로 처리중)
		void SetCamera(const Camera* pCamera) noexcept;

		void XM_CALLCONV SetWorldMatrix(FXMMATRIX w) noexcept;
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerMeshConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbPerCamera> m_cbPerCamera;
		ConstantBuffer<CbPerMesh> m_cbPerMesh;
		CbPerCamera m_cbPerCameraCache;
		CbPerMesh m_cbPerMeshCache;
	};
}
