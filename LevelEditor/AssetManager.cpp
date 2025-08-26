#include "AssetManager.h"

void AssetManager::AddATVItemMaterialHandle(HTREEITEM hItem)
{
	auto p = m_materials.insert(hItem);
	
	assert(p.second == true);
}

void AssetManager::AddATVItemTextureHandle(HTREEITEM hItem)
{
	auto p = m_textures.insert(hItem);

	assert(p.second == true);
}

void AssetManager::AddATVItemMeshHandle(HTREEITEM hItem)
{
	auto p = m_meshes.insert(hItem);

	assert(p.second == true);
}

void AssetManager::RemoveATVItemMaterialHandle(HTREEITEM hItem)
{
	size_t ret = m_materials.erase(hItem);
	
	assert(ret != 0);
}

void AssetManager::RemoveATVItemTextureHandle(HTREEITEM hItem)
{
	size_t ret = m_textures.erase(hItem);

	assert(ret != 0);
}

void AssetManager::RemoveATVItemMeshHandle(HTREEITEM hItem)
{
	size_t ret = m_meshes.erase(hItem);

	assert(ret != 0);
}

void AssetManager::RemoveAllATVItemHandle()
{
	m_materials.clear();
	m_textures.clear();
	m_meshes.clear();
}
