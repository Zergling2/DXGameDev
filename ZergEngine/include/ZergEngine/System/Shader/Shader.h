#pragma once

#include <ZergEngine\Common\EngineHeaders.h>
#include <ZergEngine\System\Shader\InputLayout.h>

#define HLSL_PAD FLOAT
#define HLSL_XMFLOATA_PADDED

namespace zergengine
{
	class MeshRenderer;
	class Skybox;
	class Terrain;

	class TextureSlotNumber
	{
	public:
		static constexpr uint32_t RENDERING_RESULT_TEX_SLOT_NUM = 0;
		static constexpr uint32_t SKYBOX_CUBE_SLOT_NUM = 0;
		static constexpr uint32_t HEIGHT_MAP_SLOT_NUM = 0;
		static constexpr uint32_t LIGHT_MAP_SLOT_NUM = 0;
		static constexpr uint32_t BASE_MAP_SLOT_NUM = 1;
		static constexpr uint32_t NORMAL_MAP_SLOT_NUM = 2;
		static constexpr uint32_t SPECULAR_MAP_SLOT_NUM = 3;
	};

	class HLSLStruct
	{
	public:
		hlsl_struct Material
		{
			inline void InitializeMtlFlag() { mtlFlag = 0; }
			inline void UseMaterial()		{ mtlFlag |= 0x00000001; }
			inline void UseLightMap()		{ mtlFlag |= 0x00000002; }
			inline void UseBaseMap()		{ mtlFlag |= 0x00000004; }
			inline void UseNormalMap()		{ mtlFlag |= 0x00000008; }
			inline void UseSpecularMap()	{ mtlFlag |= 0x00000010; }
		private:
			uint32_t mtlFlag;
			HLSL_PAD pad[3];
		public:
			// Material
			XMFLOAT4A ambient;
			XMFLOAT4A diffuse;
			XMFLOAT4A specular; // r/g/b/p
			XMFLOAT4A reflect;
		};

		hlsl_struct DirectionalLight
		{
			XMFLOAT4A ambient;
			XMFLOAT4A diffuse;
			XMFLOAT4A specular;

			XMFLOAT3 directionW;		// need to be normalized!
			HLSL_XMFLOATA_PADDED;
		};

		hlsl_struct PointLight
		{
			XMFLOAT4A ambient;
			XMFLOAT4A diffuse;
			XMFLOAT4A specular;

			XMFLOAT3 positionW;
			FLOAT range;

			XMFLOAT3 att;     // a0/a1/a2     a0 + a1d + a2d^2
			HLSL_PAD pad;
		};

		hlsl_struct SpotLight
		{
			XMFLOAT4A ambient;
			XMFLOAT4A diffuse;
			XMFLOAT4A specular;

			XMFLOAT3 positionW;
			FLOAT range;

			XMFLOAT3 directionW;
			FLOAT spotExp;

			XMFLOAT3 att;     // a0/a1/a2     a0 + a1d + a2d^2
			HLSL_PAD pad;
		};
	};

	class ConstantBuffer
	{
		friend class Graphics;
	public:
		cbuffer PerFrame
		{
			static ComPtr<ID3D11Buffer> s_cb;
			static constexpr uint32_t SLOT_NUM = 0;
			uint32_t cb_directionalLightCount;
			uint32_t cb_pointLightCount;
			uint32_t cb_spotLightCount;
			uint32_t cb_terrainMtlCount;

			FLOAT cb_terrainTextureTiling;	// 도메인 셰이더에서 Tiled 텍스쳐 좌표 계산 시 필요
			FLOAT cb_terrainCellSpacing;	// 픽셀 셰이더에서 노말 계산 시 필요
			FLOAT cb_terrainTexelSpacingU;	// 픽셀 셰이더에서 노말 계산 시 필요
			FLOAT cb_terrainTexelSpacingV;	// 픽셀 셰이더에서 노말 계산 시 필요

			HLSLStruct::Material cb_terrainMtl[5];

			HLSLStruct::DirectionalLight cb_dl[4];
			HLSLStruct::PointLight cb_pl[4];
			HLSLStruct::SpotLight cb_sl[4];
		};

		cbuffer PerCamera
		{
			static ComPtr<ID3D11Buffer> s_cb;
			static constexpr uint32_t SLOT_NUM = 1;
			XMFLOAT3 cb_camPosW;
			HLSL_PAD pad;

			FLOAT cb_tessMinDist;
			FLOAT cb_tessMaxDist;
			FLOAT cb_minTessExponent;
			FLOAT cb_maxTessExponent;

			XMFLOAT4A cb_frustumPlane[6];
		};

		cbuffer PerMesh
		{
			static ComPtr<ID3D11Buffer> s_cb;
			static constexpr uint32_t SLOT_NUM = 2;
			XMFLOAT4X4A cb_w;		// World
			XMFLOAT4X4A cb_wInvTr;	// Inversed world transform matrix (비균등 스케일링 시 올바른 노말 벡터 변환을 위해 필요)
			XMFLOAT4X4A cb_wvp;		// World * View * Proj
		};

		cbuffer PerSubset
		{
			static ComPtr<ID3D11Buffer> s_cb;
			static constexpr uint32_t SLOT_NUM = 3;
			HLSLStruct::Material cb_subsetMtl;
		};

		cbuffer PerRenderingResultMerge
		{
			static ComPtr<ID3D11Buffer> s_cb;
			static constexpr uint32_t SLOT_NUM = 4;
			FLOAT cb_width;
			FLOAT cb_height;
			FLOAT cb_topLeftX;
			FLOAT cb_topLeftY;
		};

		using PerSkybox = PerMesh;
	};

	class ShaderComponent
	{
		friend class Graphics;
	public:
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		// Input: POSITION
		class VSSkyboxTransform
		{
			friend class Graphics;
		public:
			static inline ID3D11VertexShader* GetInterface() { return s_vs.Get(); }
		private:
			static ComPtr<ID3D11VertexShader> s_vs;
		};

		// Input: POSITION, TEXCOORD0, TEXCOORD1
		class VSTerrainTransform
		{
			friend class Graphics;
		public:
			static inline ID3D11VertexShader* GetInterface() { return s_vs.Get(); }
		private:
			static ComPtr<ID3D11VertexShader> s_vs;
		};

		// Standard Transform shaders (Local Space -> Homogeneous Clip Space)

		// Input: POSITION
		class VSStandardTransformP
		{
			friend class Graphics;
		public:
			static inline ID3D11VertexShader* GetInterface() { return s_vs.Get(); }
		private:
			static ComPtr<ID3D11VertexShader> s_vs;
		};

		// Input: POSITION, COLOR
		class VSStandardTransformPC
		{
			friend class Graphics;
		public:
			static inline ID3D11VertexShader* GetInterface() { return s_vs.Get(); }
		private:
			static ComPtr<ID3D11VertexShader> s_vs;
		};

		// Input: POSITION, NORMAL
		class VSStandardTransformPN
		{
			friend class Graphics;
		public:
			static inline ID3D11VertexShader* GetInterface() { return s_vs.Get(); }
		private:
			static ComPtr<ID3D11VertexShader> s_vs;
		};

		// Input: POSITION, TEXCOORD
		class VSStandardTransformPT
		{
			friend class Graphics;
		public:
			static inline ID3D11VertexShader* GetInterface() { return s_vs.Get(); }
		private:
			static ComPtr<ID3D11VertexShader> s_vs;
		};

		// Input: POSITION, TEXCOORD
		class VSRenderingResultMerge
		{
			friend class Graphics;
		public:
			static inline ID3D11VertexShader* GetInterface() { return s_vs.Get(); }
		private:
			static ComPtr<ID3D11VertexShader> s_vs;
		};

		// Input: POSITION, NORMAL, TEXCOORD
		class VSStandardTransformPNT
		{
			friend class Graphics;
		public:
			static inline ID3D11VertexShader* GetInterface() { return s_vs.Get(); }
		private:
			static ComPtr<ID3D11VertexShader> s_vs;
		};

		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		class HSTerrainRendering
		{
			friend class Graphics;
		public:
			static inline ID3D11HullShader* GetInterface() { return s_hs.Get(); }
		private:
			static ComPtr<ID3D11HullShader> s_hs;
		};

		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		class DSTerrainRendering
		{
			friend class Graphics;
		public:
			static inline ID3D11DomainShader* GetInterface() { return s_ds.Get(); }
		private:
			static ComPtr<ID3D11DomainShader> s_ds;
		};

		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		class PSSkyboxColoring
		{
			friend class Graphics;
		public:
			static inline ID3D11PixelShader* GetInterface() { return s_ps.Get(); }
		private:
			static ComPtr<ID3D11PixelShader> s_ps;
		};

		class PSTerrainColoring
		{
			friend class Graphics;
		public:
			static inline ID3D11PixelShader* GetInterface() { return s_ps.Get(); }
		private:
			static ComPtr<ID3D11PixelShader> s_ps;
		};

		class PSStandardColoringP
		{
			friend class Graphics;
		public:
			static inline ID3D11PixelShader* GetInterface() { return s_ps.Get(); }
		private:
			static ComPtr<ID3D11PixelShader> s_ps;
		};

		class PSStandardColoringPC
		{
			friend class Graphics;
		public:
			static inline ID3D11PixelShader* GetInterface() { return s_ps.Get(); }
		private:
			static ComPtr<ID3D11PixelShader> s_ps;
		};

		class PSStandardColoringPN
		{
			friend class Graphics;
		public:
			static inline ID3D11PixelShader* GetInterface() { return s_ps.Get(); }
		private:
			static ComPtr<ID3D11PixelShader> s_ps;
		};

		class PSStandardColoringPT
		{
			friend class Graphics;
		public:
			static inline ID3D11PixelShader* GetInterface() { return s_ps.Get(); }
		private:
			static ComPtr<ID3D11PixelShader> s_ps;
		};

		class PSMSRenderingResultMerge
		{
			friend class Graphics;
		public:
			static inline ID3D11PixelShader* GetInterface() { return s_ps.Get(); }
		private:
			static ComPtr<ID3D11PixelShader> s_ps;
		};

		class PSStandardColoringPNT
		{
			friend class Graphics;
		public:
			static inline ID3D11PixelShader* GetInterface() { return s_ps.Get(); }
		private:
			static ComPtr<ID3D11PixelShader> s_ps;
		};
	};

	/*
	XMVECTOR, XMMATRIX 전달 규칙
	https://learn.microsoft.com/en-us/windows/win32/dxmath/pg-xnamath-internals
	FXMVECTOR _1, FXMVECTOR _2, FXMVECTOR _3, GXMVECTOR _4, HXMVECTOR _5, HXMVECTOR _6, CXMVECTOR _7, CXMVECTOR _8, ...
	
	Use the FXMVECTOR alias to pass up to the first three instances of XMVECTOR used as arguments to a function.
	Use the GXMVECTOR alias to pass the 4th instance of an XMVECTOR used as an argument to a function.
	Use the HXMVECTOR alias to pass the 5th and 6th instances of an XMVECTOR used as an argument to a function. For info about additional considerations, see the __vectorcall documentation.
	Use the CXMVECTOR alias to pass any further instances of XMVECTOR used as arguments.
	
	FXMMATRIX = __m128 x 4
	Use the FXMMATRIX alias to pass the first XMMATRIX as an argument to the function.
	This assumes you don't have more than two FXMVECTOR arguments or more than two float, double,
	or FXMVECTOR arguments to the 'right' of the matrix. For info about additional considerations,
	see the __vectorcall documentation.
	Use the CXMMATRIX alias otherwise. - MSDN에서 발췌
	*/

	/*
	In addition to the type aliases, you must also use the XM_CALLCONV annotation to make sure 
	the function uses the appropriate calling convention (__fastcall versus __vectorcall) based
	on your compiler and architecture. Because of limitations with __vectorcall, we recommend
	that you not use XM_CALLCONV for C++ constructors. - MSDN에서 발췌
	*/

	class StandardPEffect
	{
	public:
		static void XM_CALLCONV Render(FXMMATRIX vp, const MeshRenderer* pMeshRenderer);
	};

	class StandardPCEffect
	{
	public:
		static void XM_CALLCONV Render(FXMMATRIX vp, const MeshRenderer* pMeshRenderer);
	};

	class StandardPNEffect
	{
	public:
		static void XM_CALLCONV Render(FXMMATRIX vp, const MeshRenderer* pMeshRenderer);
	};

	class StandardPTEffect
	{
	public:
		static void XM_CALLCONV Render(FXMMATRIX vp, const MeshRenderer* pMeshRenderer);
	};

	// Position, Normal, Texcoord, Lighting
	class StandardPNTEffect
	{
	public:
		static void XM_CALLCONV Render(FXMMATRIX vp, const MeshRenderer* pMeshRenderer);
	};

	class SkyboxEffect
	{
	public:
		static void XM_CALLCONV Render(FXMMATRIX vp, FXMVECTOR camPos, const Skybox* pSkybox);
	};

	class TerrainEffect
	{
	public:
		static void XM_CALLCONV Render(FXMMATRIX vp, FXMVECTOR camPos, const Terrain* pTerrain);
	};
}
