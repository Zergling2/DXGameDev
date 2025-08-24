#pragma once

#include "framework.h"

class TerrainDataEditor
{
public:
	TerrainDataEditor()
		: m_pTerrain(nullptr)
		, m_cpStagingHeightMap()
		, m_cpStagingNormalMap()
		, m_cpStagingBlendMap()
		, m_cpStagingPatchCtrlPtBuffer()
	{
	}
	~TerrainDataEditor() = default;

	void SetTerrainToModify(ze::Terrain* pTerrain);			// pTerrain�� �����ϰ� �ִ� �ڷ����� ������ ������¡ �ڿ��� ����
	ze::Terrain* GetTerrain() const { return m_pTerrain; }

	void IncreaseHeight(size_t row, size_t column, float radius);
	void DecreaseHeight(size_t row, size_t column, float radius);
	
	// ���� �� ��ǥ�κ��� ���� �� �������� ���̸� ��ġ��Ų��.
	void Flatten(size_t row, size_t column, float radius);
private:
	ze::Terrain* m_pTerrain;
	ComPtr<ID3D11Texture2D> m_cpStagingHeightMap;
	ComPtr<ID3D11Texture2D> m_cpStagingNormalMap;
	ComPtr<ID3D11Texture2D> m_cpStagingBlendMap;
	ComPtr<ID3D11Buffer> m_cpStagingPatchCtrlPtBuffer;
};
