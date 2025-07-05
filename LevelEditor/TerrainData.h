#pragma once

#include "framework.h"

struct TERRAIN_DATA_HEADER
{
	float m_cellSize;
	float m_heightScale;
	size_t m_vertexCountRow;
	size_t m_vertexCountColumn;
};

class TerrainData
{
public:
	TerrainData(const TERRAIN_DATA_HEADER& header)
		: m_header(header)
		, m_data(header.m_vertexCountRow * header.m_vertexCountColumn)
	{
	}
	TerrainData(float cellSize, float heightScale, size_t vertexCountRow, size_t vertexCountColumn)
		: m_header{ cellSize, heightScale, vertexCountRow, vertexCountColumn }
		, m_data(vertexCountRow * vertexCountColumn)
	{
	}

	float GetCellSize() const { return m_header.m_cellSize; }
	void SetCellSize(float cellSize) { m_header.m_cellSize = cellSize; }
	float GetHeightScale() const { return m_header.m_heightScale; }
	void SetHeightScale(float heightScale) { m_header.m_heightScale = heightScale; }
	void Resize(size_t row, size_t column) { m_data.assign(row * column, 0); m_header.m_vertexCountRow = row; m_header.m_vertexCountColumn = column; }
private:
	TERRAIN_DATA_HEADER m_header;
	std::vector<uint16_t> m_data;
};
