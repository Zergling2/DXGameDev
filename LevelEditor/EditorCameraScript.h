#pragma once

#include "framework.h"

class CTerrainInspectorFormView;

class EditorCameraScript : public ze::MonoBehaviour
{
public:
	EditorCameraScript();
	virtual ~EditorCameraScript() = default;

	virtual void Start() override;
	virtual void Update() override;

	void MoveCamera();
	void EditTerrain(CTerrainInspectorFormView* pTerrainInspectorFormView);
	void UpdateMouseMovedInfo();
	bool DidMouseMoved() { return m_mouseMoved; }
private:
	ze::ComponentHandle<ze::Camera> m_camera;
	bool m_mouseMoved;
	POINT m_prevMousePos;
};
