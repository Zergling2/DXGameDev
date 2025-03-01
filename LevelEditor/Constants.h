#pragma once

class Settings
{
public:
	static constexpr COLORREF GetComponentListViewBkColor()
	{
		return RGB(200, 200, 200);
	}

	static constexpr COLORREF GetComponentListViewTextColor()
	{
		return RGB(0, 0, 0);
	}

	static constexpr COLORREF GetLogListViewBkColor()
	{
		return RGB(64, 64, 64);
	}

	static constexpr COLORREF GetLogListViewTextColor()
	{
		return RGB(220, 220, 220);
	}

	static constexpr COLORREF GetHierarchyTreeViewBkColor()
	{
		return RGB(64, 64, 64);
	}

	static constexpr COLORREF GetHierarchyTreeViewTextColor()
	{
		return RGB(220, 220, 220);
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
	TRANSFORM_ICON,
	SCRIPT_ICON,
	AUDIO_SOURCE_ICON,
	RIGIDBODY_ICON,
	PREFAB_ICON,
	CAMERA_ICON,
	COUNT
};

constexpr int GetZergEnginePackedIconSizeX()
{
	return 16;
}

constexpr int GetZergEnginePackedIconSizeY()
{
	return 16;
}

constexpr int GetZergEnginePackedIconCount()
{
	return static_cast<int>(ZERGENGINE_ICON_INDEX::COUNT);
}

constexpr COLORREF GetZergEnginePackedIconColorMask()
{
	return RGB(255, 0, 255);
}

__forceinline int GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX index)
{
	return static_cast<int>(index);
}
