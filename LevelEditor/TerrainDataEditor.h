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

	void SetTerrainToModify(ze::Terrain* pTerrain);			// pTerrain이 소유하고 있는 자료형과 동일한 스테이징 자원들 생성
	ze::Terrain* GetTerrain() const { return m_pTerrain; }

	void IncreaseHeight(size_t row, size_t column, float radius);
	void DecreaseHeight(size_t row, size_t column, float radius);
	
	// 선택 한 좌표로부터 범위 내 정점들의 높이를 일치시킨다.
	void Flatten(size_t row, size_t column, float radius);
private:
	ze::Terrain* m_pTerrain;
	ComPtr<ID3D11Texture2D> m_cpStagingHeightMap;
	ComPtr<ID3D11Texture2D> m_cpStagingNormalMap;
	ComPtr<ID3D11Texture2D> m_cpStagingBlendMap;
	ComPtr<ID3D11Buffer> m_cpStagingPatchCtrlPtBuffer;
};
