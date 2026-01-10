#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>
#include <vector>

namespace ze
{
	class Material;

	// [0] ~ [5] 지형 텍스쳐 레이어를 혼합하는 블렌드 가중치 데이터
	struct TerrainBlendMapFormat
	{
		TerrainBlendMapFormat()
			: m_layer01(0)
			, m_layer12(0)
			, m_layer23(0)
			, m_layer34(0)
		{
		}
		uint8_t m_layer01;	// 지형 텍스쳐의 0번, 1번 레이어 블렌드 가중치
		uint8_t m_layer12;	// 지형 텍스쳐의 1번, 2번 레이어 블렌드 가중치
		uint8_t m_layer23;	// 지형 텍스쳐의 2번, 3번 레이어 블렌드 가중치
		uint8_t m_layer34;	// 지형 텍스쳐의 3번, 4번 레이어 블렌드 가중치
	};

	using TerrainNormalMapFormat = XMHALF4;

	class Terrain : public IComponent
	{
		friend class Renderer;
	public:
		static constexpr ComponentType TYPE = ComponentType::Terrain;
		static constexpr bool IsCreatable() { return true; }

		Terrain();
		virtual ~Terrain() = default;

		virtual ComponentType GetType() const override { return ComponentType::Terrain; }

		float GetCellSize() const { return m_cellSize; }
		void SetCellSize(float cellSize);	// 지형 콜라이더 재생성 및 터레인 제어점 버퍼 재생성

		float GetHeightScale() const { return m_heightScale; }
		void SetHeightScale(float heightScale);	// 지형 콜라이더 재생성 및 터레인 제어점 버퍼 재생성
		float GetMaxHeight() const;

		void SetCellSizeAndHeightScale(float cellSize, float heightScale);	// 지형 콜라이더 재생성 및 터레인 제어점 버퍼 재생성

		void SetTilingScale(float scale) { m_tilingScale = scale; }
		float GetTilingScale() const { return m_tilingScale; }

		Texture2D GetHeightMap() { return m_heightMap; }
		bool SetHeightMap(Texture2D heightMap, float cellSize, float heightScale);

		Texture2D GetNormalMap() { return m_normalMap; }

		bool SetTextureLayer(Texture2D diffuseMapLayer, Texture2D normalMapLayer);

		Texture2D GetDiffuseMapLayer() const { return m_diffuseMapLayer; }
		Texture2D GetNormalMapLayer() const { return m_normalMapLayer; }

		bool SetBlendMap(Texture2D blendMap);
		Texture2D GetBlendMap() const { return m_blendMap; }

		ID3D11Buffer* GetPatchControlPointVertexBuffer() const { return m_cpPatchCtrlPtBuffer.Get(); }
		ID3D11Buffer* GetPatchControlPointIndexBuffer() const { return m_cpPatchCtrlPtIndexBuffer.Get(); }
		uint32_t GetPatchControlPointIndexCount() const { return m_patchCtrlPtIndexCount; }

		const float* GetHeightData() const { return m_heightData.data(); }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		float m_cellSize;
		float m_heightScale;
		float m_tilingScale;
		uint32_t m_patchCtrlPtIndexCount;
		Texture2D m_heightMap;
		Texture2D m_normalMap;	// HeightMap에 대한 노말 맵
		Texture2D m_diffuseMapLayer;
		Texture2D m_normalMapLayer;
		Texture2D m_blendMap;			// Texture blend map
		ComPtr<ID3D11Buffer> m_cpPatchCtrlPtBuffer;
		ComPtr<ID3D11Buffer> m_cpPatchCtrlPtIndexBuffer;
		std::vector<float> m_heightData;	// Height data on the system memory
	};
}
