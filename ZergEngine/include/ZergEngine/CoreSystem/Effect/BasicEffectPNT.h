#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class Camera;
	class Texture2D;

	class BasicEffectPNT : public IEffect
	{
	private:
		enum DIRTY_FLAG : DWORD
		{
			PRIMITIVE_TOPOLOGY			= 1 << 0,
			INPUT_LAYOUT				= 1 << 1,
			SHADER						= 1 << 2,
			CONSTANTBUFFER_PER_FRAME	= 1 << 3,
			CONSTANTBUFFER_PER_CAMERA	= 1 << 4,
			CONSTANTBUFFER_PER_MESH		= 1 << 5,
			CONSTANTBUFFER_PER_SUBSET	= 1 << 6,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		BasicEffectPNT() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerFrame()
			, m_cbPerCamera()
			, m_cbPerMesh()
			, m_cbPerSubset()
			, m_cbPerFrameCache()
			, m_cbPerCameraCache()
			, m_cbPerMeshCache()
			, m_cbPerSubsetCache()
			, m_pTextureSRVArray{ nullptr, nullptr, nullptr, nullptr }
		{
		}
		virtual ~BasicEffectPNT() = default;

		virtual void Init() override;
		virtual void Release() override;

		// count�� 4���� ���ų� �۾ƾ� �մϴ�.
		void SetDirectionalLight(const DirectionalLightData* pLights, uint32_t count) noexcept;
		void SetPointLight(const PointLightData* pLights, uint32_t count) noexcept;
		void SetSpotLight(const SpotLightData* pLights, uint32_t count) noexcept;

		// View Matrix, Projection Matrix�� ������Ʈ �� ���·� ���޵Ǿ�� ��. (=> ���� ��Ÿ�ӿ��� �ڵ����� ó����)
		void SetCamera(const Camera* pCamera) noexcept;
		
		void XM_CALLCONV SetWorldMatrix(FXMMATRIX w) noexcept;

		void UseMaterial(bool b) noexcept;
		void XM_CALLCONV SetAmbientColor(FXMVECTOR ambient) noexcept;
		void XM_CALLCONV SetDiffuseColor(FXMVECTOR diffuse) noexcept;
		void XM_CALLCONV SetSpecularColor(FXMVECTOR specular) noexcept;
		void XM_CALLCONV SetReflection(FXMVECTOR reflect) noexcept;

		void SetLightMap(const Texture2D& lightMap) noexcept;
		void SetDiffuseMap(const Texture2D& diffuseMap) noexcept;
		void SetNormalMap(const Texture2D& normalMap) noexcept;
		void SetSpecularMap(const Texture2D& specularMap) noexcept;
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerFrameConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerMeshConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerSubsetConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;

		void ClearTextureSRVArray()
		{
			for (size_t i = 0; i < _countof(m_pTextureSRVArray); ++i)
				m_pTextureSRVArray[i] = nullptr;
		}
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbPerFrame> m_cbPerFrame;
		ConstantBuffer<CbPerCamera> m_cbPerCamera;
		ConstantBuffer<CbPerMesh> m_cbPerMesh;
		ConstantBuffer<CbPerSubset> m_cbPerSubset;
		CbPerFrame m_cbPerFrameCache;
		CbPerCamera m_cbPerCameraCache;
		CbPerMesh m_cbPerMeshCache;
		CbPerSubset m_cbPerSubsetCache;

		// [0] LIGHT_MAP, [1] DIFFUSE_MAP, [2] NORMAL_MAP, [3] SPECULAR_MAP
		ID3D11ShaderResourceView* m_pTextureSRVArray[4];
	};
}
