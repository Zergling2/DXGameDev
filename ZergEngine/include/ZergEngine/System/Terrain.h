#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	enum class HEIGHT_MAP_FORMAT : uint16_t
	{
		RAW_8BIT = 1,
		RAW_16BIT = 2
	};

	// Max: 32769 x 32769
	struct HeightMapSize
	{
		uint16_t row;
		uint16_t column;
	};

	struct TerrainMapDescriptor
	{
		TerrainMapDescriptor()
			: cellSpacing(1.0f)
			, heightBase(0.0f)
			, heightScale(10.0f)
			, textureScale(32.0f)
		{
		}
		HeightMapSize heightMapSize;
		float cellSpacing;
		float heightBase;
		float heightScale;
		float textureScale;
		HEIGHT_MAP_FORMAT elementFormat;
	};

	class Terrain
	{
		friend class Resource;
	public:
		Terrain(const TerrainMapDescriptor& tmd);
		~Terrain();

		inline uint16_t GetHeightMapTextureRowElements() const { return m_tmd.heightMapSize.row; }
		inline uint16_t GetHeightMapTextureColumnElements() const { return m_tmd.heightMapSize.column; }

		// Spacing between cells
		inline float GetCellSpacing() const { return m_tmd.cellSpacing; }

		// Base height value
		inline float GetHeightBase() const { return m_tmd.heightBase; }

		// Scale along the Y axis
		inline float GetHeightScale() const { return m_tmd.heightScale; }

		// How much to tile the texture layers
		inline float GetTextureScale() const { return m_tmd.textureScale; }

		// Length along X axis
		inline float GetTerrainWidth() const { return static_cast<float>(m_tmd.heightMapSize.column - 1) * m_tmd.cellSpacing; }

		// Length along Z axis
		inline float GetTerrainDepth() const { return static_cast<float>(m_tmd.heightMapSize.row - 1) * m_tmd.cellSpacing; }

		inline float GetTexelSpacingU() const { return 1.0f / static_cast<float>(m_tmd.heightMapSize.column - 1); }
		inline float GetTexelSpacingV() const { return 1.0f / static_cast<float>(m_tmd.heightMapSize.row - 1); }

		inline ID3D11Buffer* GetPatchControlPointBufferInterface() const { return m_patchCtrlPtBuffer.Get(); }
		inline ID3D11Buffer* GetPatchControlPointIndexBufferInterface() const { return m_patchCtrlPtIndexBuffer.Get(); }
		inline ID3D11ShaderResourceView* GetHeightMapShaderResourceView() const { return m_srvHeightMap.Get(); }
		inline uint32_t GetPatchControlPointIndexCount() const { return m_patchCtrlPtIndexCount; }
	private:
		const TerrainMapDescriptor m_tmd;
		const uint16_t m_numPatchVertexRow;
		const uint16_t m_numPatchVertexCol;
		const uint32_t m_patchCtrlPtIndexCount;
		float* m_pHeightData;
		ComPtr<ID3D11Buffer> m_patchCtrlPtBuffer;
		ComPtr<ID3D11Buffer> m_patchCtrlPtIndexBuffer;
		ComPtr<ID3D11ShaderResourceView> m_srvHeightMap;
	};
}
