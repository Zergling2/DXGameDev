#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Texture2D;

	class TerrainData
	{
	public:
		TerrainData();
		~TerrainData() = default;

		// Number of pixels
		inline uint32_t GetHeightmapWidth() const { return m_heightmapWidth; }
		inline uint32_t GetHeightmapHeight() const { return m_heightmapHeight; }

		// How much to tile the terrain texture
		inline void SetUVScale(float scale) { m_uvScale = scale; }

		// Length along X axis
		inline float GetTerrainSizeAlongX() const { return static_cast<float>(GetHeightmapWidth() - 1) * m_heightmapScale.x; }

		// Length along Z axis
		inline float GetTerrainSizeAlongZ() const { return static_cast<float>(GetHeightmapHeight() - 1) * m_heightmapScale.z; }

		inline float GetTexelSpacingU() const { return 1.0f / static_cast<float>(GetHeightmapWidth() - 1); }
		inline float GetTexelSpacingV() const { return 1.0f / static_cast<float>(GetHeightmapHeight() - 1); }

		inline ID3D11Buffer* GetPatchControlPointBufferInterface() const { return m_cpPatchCtrlPtBuffer.Get(); }
		inline ID3D11Buffer* GetPatchControlPointIndexBufferInterface() const { return m_cpPatchCtrlPtIndexBuffer.Get(); }
		inline uint32_t GetPatchControlPointIndexCount() const { return m_patchCtrlPtIndexCount; }

		bool SetHeightData(const std::vector<float>& heightData);
		bool SetHeightData(std::vector<float>&& heightData);

		bool SetTerrainLayer(const std::shared_ptr<Texture2D> texture, size_t index);
		std::shared_ptr<Texture2D> GetTerrainLayer(size_t index);

		bool SetBlendmap(const std::shared_ptr<Texture2D> blendmap);	// 포맷 확인
	private:
		XMFLOAT3 m_heightmapScale;
		FLOAT m_uvScale;		// How much to tile the terrain texture
		std::shared_ptr<Texture2D> m_spDiffusemap[5];
		std::shared_ptr<Texture2D> m_spNormalmap[5];
		std::shared_ptr<Texture2D> m_spBlendmap;			// Terrain texture blendmap

		uint32_t m_heightmapWidth;
		uint32_t m_heightmapHeight;
		uint32_t m_patchCtrlPtCountRow;
		uint32_t m_patchCtrlPtCountCol;
		uint32_t m_patchCtrlPtIndexCount;
		std::vector<float> m_heightData;	// system memory
		ComPtr<ID3D11ShaderResourceView> m_cpHeightmapSRV;
		ComPtr<ID3D11Buffer> m_cpPatchCtrlPtBuffer;
		ComPtr<ID3D11Buffer> m_cpPatchCtrlPtIndexBuffer;
	};
}
