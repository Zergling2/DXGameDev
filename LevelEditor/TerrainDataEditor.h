#pragma once

#include "framework.h"
#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class Terrain;
	class Ray;
}

class TerrainDataEditor
{
public:
	TerrainDataEditor()
		: m_pTerrainToModify(nullptr)
		, m_cpStagingHeightMap()
		, m_cpStagingNormalMap()
		, m_cpStagingBlendMap()
		, m_cpStagingPatchCtrlPtBuffer()
	{
	}
	~TerrainDataEditor() = default;

	void SetTerrainToModify(ze::Terrain* pTerrain);			// pTerrain�� �����ϰ� �ִ� �ڷ����� ������ ������¡ �ڿ��� ����

	void RaiseOrLowerTerrain(bool raise, const ze::Ray& ray, float radius, float opacity);

	void PaintTexture(const ze::Ray& ray, float radius, uint8_t opacity);

	// opacity: Ÿ�� ���̿� �����ϴ� �ӵ�
	void SetHeight(const ze::Ray& ray, float radius, uint16_t height, float opacity);

	void SmoothHeight(const ze::Ray& ray, float radius, float opacity);

	ID3D11Texture2D* GetStagingHeightMapComInterface() const { return m_cpStagingHeightMap.Get(); }
	ID3D11Texture2D* GetStagingNormalMapComInterface() const { return m_cpStagingNormalMap.Get(); }
	ID3D11Texture2D* GetStagingBlendMapComInterface() const { return m_cpStagingBlendMap.Get(); }
	ID3D11Buffer* GetStagingPatchCtrlPtBufferComInterface() const { return m_cpStagingPatchCtrlPtBuffer.Get(); }
private:
	ze::Terrain* m_pTerrainToModify;
	ComPtr<ID3D11Texture2D> m_cpStagingHeightMap;
	ComPtr<ID3D11Texture2D> m_cpStagingNormalMap;
	ComPtr<ID3D11Texture2D> m_cpStagingBlendMap;
	ComPtr<ID3D11Buffer> m_cpStagingPatchCtrlPtBuffer;
};
