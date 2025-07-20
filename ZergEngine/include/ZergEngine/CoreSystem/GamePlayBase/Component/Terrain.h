#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

namespace ze
{
	class Terrain : public IComponent
	{
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::TERRAIN;
		static constexpr bool IsCreatable() { return true; }

		Terrain(uint8_t patchCountRow, uint8_t patchCountCol, float cellSize, float heightScale);
		virtual ~Terrain() = default;

		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::TERRAIN; }

		// Number of pixels
		uint32_t GetHeightmapWidth() const { return m_heightMapWidth; }
		uint32_t GetHeightmapHeight() const { return m_heightMapHeight; }

		// How much to tile the terrain texture
		void SetTilingScale(float val) { m_tilingScale = val; }
		float GetTilingScale() const { return m_tilingScale; }

		// Length along X axis
		float GetTerrainSizeAlongX() const { return static_cast<float>(GetHeightmapWidth() - 1) * m_heightMapScale.x; }

		// Length along Z axis
		float GetTerrainSizeAlongZ() const { return static_cast<float>(GetHeightmapHeight() - 1) * m_heightMapScale.z; }

		float GetTexelSpacingU() const { return 1.0f / static_cast<float>(GetHeightmapWidth() - 1); }
		float GetTexelSpacingV() const { return 1.0f / static_cast<float>(GetHeightmapHeight() - 1); }

		ID3D11Buffer* GetPatchControlPointBufferInterface() const { return m_cpPatchCtrlPtBuffer.Get(); }
		ID3D11Buffer* GetPatchControlPointIndexBufferInterface() const { return m_cpPatchCtrlPtIndexBuffer.Get(); }
		uint32_t GetPatchControlPointIndexCount() const { return m_patchCtrlPtIndexCount; }

		bool SetHeightData(const std::vector<float>& heightData);
		bool SetHeightData(std::vector<float>&& heightData);

		bool SetTerrainLayer(const Texture2D& texture, size_t index);
		Texture2D GetTerrainLayer(size_t index);

		bool SetBlendmap(Texture2D& blendmap);	// 포맷 확인
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		XMFLOAT3 m_heightMapScale;
		FLOAT m_tilingScale;				// How much to tile the terrain texture
		Texture2D m_diffuseMap[5];
		Texture2D m_normalMap[5];
		Texture2D m_blendMap;			// Diffuse texture blend map

		const uint32_t m_heightMapWidth;
		const uint32_t m_heightMapHeight;
		const uint32_t m_patchCtrlPtCountRow;
		const uint32_t m_patchCtrlPtCountCol;
		const uint32_t m_patchCtrlPtIndexCount;
		std::vector<FLOAT> m_heightData;	// system memory
		ComPtr<ID3D11ShaderResourceView> m_cpHeightMapSRV;
		ComPtr<ID3D11Buffer> m_cpPatchCtrlPtBuffer;
		ComPtr<ID3D11Buffer> m_cpPatchCtrlPtIndexBuffer;
	};
}
