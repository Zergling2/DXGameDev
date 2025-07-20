#pragma once

#include "framework.h"

class TerrainEditor
{
public:
	TerrainEditor()
		: m_pTerrain(nullptr)
	{
	}
	~TerrainEditor() = default;

	void SetDataToModify(ze::Terrain* pTerrain) { m_pTerrain = pTerrain; }
	ze::Terrain* GetData() const { return m_pTerrain; }

	void IncreaseHeight(size_t row, size_t column, float radius);
	void DecreaseHeight(size_t row, size_t column, float radius);
	
	// ���� �� ��ǥ�κ��� ���� �� �������� ���̸� ��ġ��ŵ�ϴ�.
	void Flatten(size_t row, size_t column, float radius);
private:
	ze::Terrain* m_pTerrain;
};
