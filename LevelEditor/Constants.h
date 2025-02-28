#pragma once

class Settings
{
public:
	static constexpr COLORREF GetListBkColor()
	{
		return RGB(200, 200, 200);
	}
};

enum class ZERGENGINE_ICON_INDEX : int
{
	ENGINE_LOGO_ICON = 0,
	ERROR_ICON,
	WARNING_ICON,
	INFO_ICON,
	GAMEOBJECT_ICON,
	MESH_FILTER_ICON,
	MESH_RENDERER_ICON,
	COUNT
};

constexpr int GetZergEnginePackedIconCount()
{
	return static_cast<int>(ZERGENGINE_ICON_INDEX::COUNT);
}

constexpr COLORREF GetZergEnginePackedIconColorMask()
{
	return RGB(255, 255, 255);
}

__forceinline int GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX index)
{
	return static_cast<int>(index);
}
