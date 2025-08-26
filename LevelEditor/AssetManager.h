#pragma once

#include "framework.h"

class ATVItemMaterial;
class ATVItemTexture;
class ATVItemMesh;

class AssetManager
{
public:
	AssetManager() = default;
	~AssetManager() = default;

	void AddATVItemMaterialHandle(HTREEITEM hItem);
	void AddATVItemTextureHandle(HTREEITEM hItem);
	void AddATVItemMeshHandle(HTREEITEM hItem);
	void RemoveATVItemMaterialHandle(HTREEITEM hItem);
	void RemoveATVItemTextureHandle(HTREEITEM hItem);
	void RemoveATVItemMeshHandle(HTREEITEM hItem);
	void RemoveAllATVItemHandle();
	const std::unordered_set<HTREEITEM>& GetMaterialSet() const { return m_materials; }
	const std::unordered_set<HTREEITEM>& GetTextureSet() const { return m_textures; }
	const std::unordered_set<HTREEITEM>& GetMeshSet() const { return m_meshes; }
private:
	std::unordered_set<HTREEITEM> m_materials;
	std::unordered_set<HTREEITEM> m_textures;
	std::unordered_set<HTREEITEM> m_meshes;
};
