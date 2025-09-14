#include "AssetManager.h"

void AssetManager::AddATVItemMaterialHandle(HTREEITEM hItem)
{
	auto p = m_materials.insert(hItem);
	
	ASSERT(p.second == true);
}

void AssetManager::AddATVItemTextureHandle(HTREEITEM hItem)
{
	auto p = m_textures.insert(hItem);

	ASSERT(p.second == true);
}

void AssetManager::AddATVItemMeshHandle(HTREEITEM hItem)
{
	auto p = m_meshes.insert(hItem);

	ASSERT(p.second == true);
}

void AssetManager::RemoveATVItemMaterialHandle(HTREEITEM hItem)
{
	size_t ret = m_materials.erase(hItem);
	
	ASSERT(ret != 0);
}

void AssetManager::RemoveATVItemTextureHandle(HTREEITEM hItem)
{
	size_t ret = m_textures.erase(hItem);

	ASSERT(ret != 0);
}

void AssetManager::RemoveATVItemMeshHandle(HTREEITEM hItem)
{
	size_t ret = m_meshes.erase(hItem);

	ASSERT(ret != 0);
}

void AssetManager::RemoveAllATVItemHandle()
{
	m_materials.clear();
	m_textures.clear();
	m_meshes.clear();
}
