#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class Camera;

	class TerrainEffect : public IEffect
	{
	private:
		enum DIRTY_FLAG : DWORD
		{
			PRIMITIVE_TOPOLOGY				= 1 << 0,
			INPUT_LAYOUT					= 1 << 1,
			SHADER							= 1 << 2,
			CONSTANTBUFFER_PER_FRAME		= 1 << 3,
			CONSTANTBUFFER_PER_CAMERA		= 1 << 4,
			CONSTANTBUFFER_PER_TERRAIN		= 1 << 5,

			COUNT,

			ALL								= ((COUNT - 1) << 1) - 1
		};
	public:
		TerrainEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pHullShader(nullptr)
			, m_pDomainShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerFrame()
			, m_cbPerCamera()
			, m_cbPerTerrain()
			, m_cbPerFrameCache()
			, m_cbPerCameraCache()
			, m_cbPerTerrainCache()
			, m_pHeightMapSRV(nullptr)
			, m_pNormalMapSRV(nullptr)
			, m_pDiffuseMapLayerSRV(nullptr)
			, m_pNormalMapLayerSRV(nullptr)
			, m_pBlendMapSRV(nullptr)
		{
		}
		virtual ~TerrainEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void XM_CALLCONV SetAmbientLight(FXMVECTOR ambientLight) noexcept;

		// count는 4보다 같거나 작아야 합니다.
		void SetDirectionalLight(const DirectionalLightData* pLights, uint32_t count) noexcept;
		void SetPointLight(const PointLightData* pLights, uint32_t count) noexcept;
		void SetSpotLight(const SpotLightData* pLights, uint32_t count) noexcept;

		// View Matrix, Projection Matrix가 업데이트 된 상태로 전달되어야 함. (=> 엔진 런타임에서 자동으로 처리중)
		void SetCamera(const Camera* pCamera) noexcept;
		void SetMaxHeight(float maxHeight) noexcept;
		void SetTilingScale(float tilingScale) noexcept;
		void SetFieldMap(ID3D11ShaderResourceView* pHeightMapSRV, ID3D11ShaderResourceView* pNormalMapSRV) noexcept;
		void SetLayerTexture(
			ID3D11ShaderResourceView* pDiffuseMapLayerSRV,
			ID3D11ShaderResourceView* pNormalMapLayerSRV,
			ID3D11ShaderResourceView* pBlendMapSRV
		) noexcept;
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerFrameConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerTerrainConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11HullShader* m_pHullShader;
		ID3D11DomainShader* m_pDomainShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbPerForwardRenderingFrame> m_cbPerFrame;
		ConstantBuffer<CbPerCamera> m_cbPerCamera;
		ConstantBuffer<CbPerTerrain> m_cbPerTerrain;
		CbPerForwardRenderingFrame m_cbPerFrameCache;
		CbPerCamera m_cbPerCameraCache;
		CbPerTerrain m_cbPerTerrainCache;

		ID3D11ShaderResourceView* m_pHeightMapSRV;
		ID3D11ShaderResourceView* m_pNormalMapSRV;
		ID3D11ShaderResourceView* m_pDiffuseMapLayerSRV;
		ID3D11ShaderResourceView* m_pNormalMapLayerSRV;
		ID3D11ShaderResourceView* m_pBlendMapSRV;
	};
}
