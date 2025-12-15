#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class Camera;
	class Material;

	class BasicEffectPNTTSkinned : public IEffect
	{
		enum DIRTY_FLAG : DWORD
		{
			PRIMITIVE_TOPOLOGY			= 1 << 0,
			INPUT_LAYOUT				= 1 << 1,
			SHADER						= 1 << 2,
			PIXEL_SHADER				= 1 << 3,
			CONSTANTBUFFER_PER_FRAME	= 1 << 4,
			CONSTANTBUFFER_PER_CAMERA	= 1 << 5,
			CONSTANTBUFFER_PER_MESH		= 1 << 6,
			CONSTANTBUFFER_PER_ARMATURE	= 1 << 7,
			CONSTANTBUFFER_PER_SUBSET	= 1 << 8,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		BasicEffectPNTTSkinned() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVS(nullptr)
			, m_pCurrPS(nullptr)
			, m_pPSUnlitPNTTNoMtl(nullptr)
			, m_cbPerFrame()
			, m_cbPerCamera()
			, m_cbPerMesh()
			, m_cbPerSubset()
			, m_cbPerFrameCache()
			, m_cbPerCameraCache()
			, m_cbPerMeshCache()
			, m_cbPerArmatureCache()
			, m_cbPerSubsetCache()
			, m_pTextureSRVArray{ nullptr, nullptr, nullptr }
		{
			ZeroMemory(m_psTable, sizeof(m_psTable));
		}
		virtual ~BasicEffectPNTTSkinned() = default;

		virtual void Init() override;
		virtual void Release() override;

		void XM_CALLCONV SetAmbientLight(FXMVECTOR ambientLight) noexcept;

		// count는 4보다 같거나 작아야 합니다.
		void SetDirectionalLight(const DirectionalLightData* pLights, uint32_t count) noexcept;
		void SetPointLight(const PointLightData* pLights, uint32_t count) noexcept;
		void SetSpotLight(const SpotLightData* pLights, uint32_t count) noexcept;

		// View Matrix, Projection Matrix가 업데이트 된 상태로 전달되어야 함. (=> 엔진 런타임에서 자동으로 처리중)
		void SetCamera(const Camera* pCamera) noexcept;

		void XM_CALLCONV SetWorldMatrix(FXMMATRIX w) noexcept;

		// Copy
		void SetArmatureFinalTransform(const XMFLOAT4X4A* pFinalTransforms, size_t count);

		void SetMaterial(const Material* pMaterial);	// nullable
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPixelShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerFrameConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerMeshConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerArmatureConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerSubsetConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ClearTextureSRVArray() noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVS;
		ID3D11PixelShader* m_pCurrPS;
		ID3D11PixelShader* m_pPSUnlitPNTTNoMtl;
		ID3D11PixelShader* m_psTable[2][2][2];

		ConstantBuffer<CbPerForwardRenderingFrame> m_cbPerFrame;
		ConstantBuffer<CbPerCamera> m_cbPerCamera;
		ConstantBuffer<CbPerMesh> m_cbPerMesh;
		ConstantBuffer<CbPerArmature> m_cbPerArmature;
		ConstantBuffer<CbPerSubset> m_cbPerSubset;
		CbPerForwardRenderingFrame m_cbPerFrameCache;
		CbPerCamera m_cbPerCameraCache;
		CbPerMesh m_cbPerMeshCache;
		CbPerArmature m_cbPerArmatureCache;
		CbPerSubset m_cbPerSubsetCache;

		// [0] DIFFUSE_MAP, [1] SPECULAR_MAP, [2] NORMAL_MAP
		ID3D11ShaderResourceView* m_pTextureSRVArray[3];
	};
}
