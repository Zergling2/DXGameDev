#pragma once

#include "..\framework.h"

enum class ATVItemType
{
	Empty,
	Folder,
	Material,
	Texture,
	StaticMesh,
	Animation,
	SkinnedMesh
};

class IATVItem abstract
{
public:
	IATVItem() = default;
	virtual ~IATVItem() = default;

	virtual ATVItemType GetType() const = 0;
	virtual void OnSelect() = 0;
};
