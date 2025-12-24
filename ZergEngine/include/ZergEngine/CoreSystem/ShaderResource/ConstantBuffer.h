#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\EngineConstants.h>

#define hlslstruct struct alignas(16)
#define HLSLPad float

namespace ze
{
	class IConstantBuffer abstract
	{
	public:
		IConstantBuffer()
			: m_cpBuffer()
		{
		}
		virtual ~IConstantBuffer() = default;

		void Release();
		ID3D11Buffer* GetComInterface() const { return m_cpBuffer.Get(); }
	protected:
		void InitImpl(ID3D11Device* pDevice, size_t bufferSize);
		void UpdateImpl(ID3D11DeviceContext* pDeviceContext, const void* pData, size_t size);
	protected:
		ComPtr<ID3D11Buffer> m_cpBuffer;
	};

	template<typename T>
	class ConstantBuffer : public IConstantBuffer
	{
	public:
		ConstantBuffer() = default;
		virtual ~ConstantBuffer() = default;

		void Init(ID3D11Device* pDevice) { InitImpl(pDevice, sizeof(T)); }
		void Update(ID3D11DeviceContext* pDeviceContext, const T* pData) { UpdateImpl(pDeviceContext, pData, sizeof(T)); }
	};

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃                CONSTANT BUFFER                ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

	/*
	enum class MaterialFlag : uint32_t
	{
		None			= 0,
		UseMaterial		= 0x80000000,
		UseDiffuseMap	= 0x00000001,
		UseSpecularMap	= 0x00000002,
		UseNormalMap	= 0x00000004
	};
	*/

	/*
	enum class TerrainLayerFlag : uint32_t
	{
		None				= 0,
		UseDiffuseLayer		= 0x00000001,
		UseSpecularLayer	= 0x00000002,
		UseNormalLayer		= 0x00000004
	};
	*/

	hlslstruct MaterialData
	{
		MaterialData()
		{
#if defined(DEBUG) || defined(_DEBUG)
			XMStoreFloat4A(&diffuse, XMVectorZero());
			XMStoreFloat4A(&specular, XMVectorZero());
			XMStoreFloat4A(&reflect, XMVectorZero());
#endif
		}
	public:
		// Material
		XMFLOAT4A diffuse;
		XMFLOAT4A specular; // r/g/b/p
		XMFLOAT4A reflect;
	};

	hlslstruct DirectionalLightData
	{
		DirectionalLightData()
		{
#if defined(DEBUG) || defined(_DEBUG)
			XMStoreFloat4A(&diffuse, XMVectorZero());
			XMStoreFloat4A(&specular, XMVectorZero());

			XMStoreFloat3(&directionW, XMVectorZero());
#endif
		}
		XMFLOAT4A diffuse;
		XMFLOAT4A specular;

		XMFLOAT3 directionW;	// Need to be normalized!
		HLSLPad pad0;
	};

	hlslstruct PointLightData
	{
		PointLightData()
		{
#if defined(DEBUG) || defined(_DEBUG)
			XMStoreFloat4A(&diffuse, XMVectorZero());
			XMStoreFloat4A(&specular, XMVectorZero());

			XMStoreFloat3(&positionW, XMVectorZero());
			range = 10.0f;

			XMStoreFloat3(&att, XMVectorZero());	// 0, 0, 1, 0
#endif
		}
		XMFLOAT4A diffuse;
		XMFLOAT4A specular;

		XMFLOAT3 positionW;
		FLOAT range;

		XMFLOAT3 att;     // a0/a1/a2     a0 + a1d + a2d^2
		HLSLPad pad0;
	};

	hlslstruct SpotLightData
	{
		SpotLightData()
		{
#if defined(DEBUG) || defined(_DEBUG)
			XMStoreFloat4A(&diffuse, XMVectorZero());
			XMStoreFloat4A(&specular, XMVectorZero());

			XMStoreFloat3(&positionW, XMVectorZero());
			range = 0.0f;

			XMStoreFloat3(&directionW, XMVectorZero());
			innerConeCos = 0.0f;

			XMStoreFloat3(&att, XMVectorZero());
			outerConeCos = 0.0f;
#endif
		}
		XMFLOAT4A diffuse;
		XMFLOAT4A specular;

		XMFLOAT3 positionW;
		FLOAT range;

		XMFLOAT3 directionW;	// Need to be normalized!
		FLOAT innerConeCos;

		XMFLOAT3 att;     // a0/a1/a2     a0 + a1d + a2d^2
		FLOAT outerConeCos;
	};

	hlslstruct CbPerForwardRenderingFrame
	{
		uint32_t dlCount;
		uint32_t plCount;
		uint32_t slCount;
		HLSLPad pad0;

		XMFLOAT3 ambientLight;
		HLSLPad pad1;

		DirectionalLightData dl[MAX_GLOBAL_LIGHT_COUNT];
		PointLightData pl[MAX_GLOBAL_LIGHT_COUNT];
		SpotLightData sl[MAX_GLOBAL_LIGHT_COUNT];
	};

	hlslstruct CbPerDeferredRenderingFrame
	{
		XMFLOAT3 ambientLight;
		HLSLPad pad0;
	};

	hlslstruct CbPerCamera
	{
		XMFLOAT3 cameraPosW;
		HLSLPad pad0;

		FLOAT tessMinDist;
		FLOAT tessMaxDist;
		FLOAT minTessExponent;
		FLOAT maxTessExponent;

		XMFLOAT4A worldSpaceFrustumPlane[6];

		XMFLOAT4X4A vp;		// View * Proj
	};

	hlslstruct CbPerMesh
	{
		XMFLOAT4X4A w;			// World
		XMFLOAT4X4A wInvTr;		// Inversed world transform matrix (비균등 스케일링 시 올바른 노말 벡터 변환을 위해 필요)
	};

	hlslstruct CbShadowMapInfo
	{
		FLOAT shadowMapSize;
		FLOAT shadowMapTexelSize;
	};

	hlslstruct CbPerTerrain
	{
		FLOAT maxHeight;
		FLOAT tilingScale;
		uint32_t layerArraySize;
		HLSLPad pad0;
	};

	hlslstruct CbPerScreenRatioQuad
	{
		FLOAT width;
		FLOAT height;
		FLOAT topLeftX;
		FLOAT topLeftY;
	};

	hlslstruct CbMaterial
	{
		MaterialData mtl;
	};

	hlslstruct CbPerBillboard
	{
		CbPerBillboard()
			: uvOffset(0.0f, 0.0f)
			, uvScale(1.0f, 1.0f)
		{
#if defined(DEBUG) || defined(_DEBUG)
			XMStoreFloat4x4A(&w, XMMatrixIdentity());
			XMStoreFloat4x4A(&wInvTr, XMMatrixIdentity());
#endif
		}
		XMFLOAT4X4A w;
		XMFLOAT4X4A wInvTr;

		// 텍스쳐 팔레트용 정보
		XMFLOAT2 uvOffset;	// UV 위치 오프셋
		XMFLOAT2 uvScale;	// UV 스케일
	};

	hlslstruct Cb2DRender
	{
		XMFLOAT2 toNDCSpaceRatio;	// 
	};

	hlslstruct CbPer2DQuad
	{
		CbPer2DQuad()
			: size(1.0f, 1.0f)
			, position(0.0f, 0.0f)
			, uvOffset(0.0f, 0.0f)
			, uvScale(1.0f, 1.0f)
			, color(1.0f, 1.0f, 1.0f, 1.0f)
			, ltColorWeight(0.0f)
			, rbColorWeight(0.0f)
		{
		}
		XMFLOAT2 size;
		XMFLOAT2 position;	// View space position

		// 텍스쳐 팔레트용 정보
		XMFLOAT2 uvOffset;	// UV 위치 오프셋
		XMFLOAT2 uvScale;	// UV 스케일

		XMFLOAT4A color;

		FLOAT ltColorWeight;	// 음영 쿼드 렌더링 시 사용
		FLOAT rbColorWeight;	// 음영 쿼드 렌더링 시 사용
	};

	/*
	constexpr uint32_t SHADED_2DQUAD_COLOR_WEIGHT_INDEX_CONVEX = 0;
	constexpr uint32_t SHADED_2DQUAD_COLOR_WEIGHT_INDEX_CONCAVE = 1;
	hlslstruct CbPerShaded2DQuad
	{
		XMFLOAT4A color;

		XMFLOAT2 size;
		XMFLOAT2 position;	// View space position

		uint32_t colorWeightIndex;		// [0] Convex shade [1] Concave shade
	};
	*/

	hlslstruct CbPerCheckbox
	{
		XMFLOAT4A boxColor;
		XMFLOAT4A checkColor;

		XMFLOAT2 size;
		XMFLOAT2 position;	// View space position
	};

	hlslstruct CbPerArmature
	{
		XMFLOAT4X4A finalTransform[MAX_BONE_COUNT];
	};
}
