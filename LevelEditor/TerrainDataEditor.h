#pragma once

class TerrainData;

class TerrainEditor
{
public:
	TerrainEditor()
		: m_pTerrainData(nullptr)
	{
	}
	~TerrainEditor() = default;

	void SetDataToModify(TerrainData* pTerrainData) { m_pTerrainData; }
	TerrainData* GetData() const { return m_pTerrainData; }

	void IncreaseHeight(size_t row, size_t column, float radius);
	void DecreaseHeight(size_t row, size_t column, float radius);
	
	// 선택 한 좌표로부터 범위 내 정점들의 높이를 일치시킵니다.
	void Flatten(size_t row, size_t column, float radius);
private:
	TerrainData* m_pTerrainData;
};
