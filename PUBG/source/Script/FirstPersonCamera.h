#pragma once

#include <ZergEngine\ZergEngine.h>

namespace pubg
{
	class FirstPersonCamera : public ze::MonoBehaviour
	{
		DECLARE_MONOBEHAVIOUR_TYPE
	public:
		FirstPersonCamera()
			: m_onUI(false)
			, m_hPanel()
		{
		}
		virtual ~FirstPersonCamera() = default;

		virtual void Awake() override { printf("FirstPersonCamera::Awake()\n"); }
		virtual void OnEnable() override { printf("FirstPersonCamera::OnEnable()\n"); }
		virtual void Start() override { printf("FirstPersonCamera::Start()\n"); }
		virtual void Update() override;
		virtual void OnDisable() override { printf("FirstPersonCamera::OnDisable()\n"); }
		virtual void OnDestroy() override { printf("FirstPersonCamera::OnDestroy()\n"); }
	public:
		bool m_onUI;
		ze::UIObjectHandle m_hPanel;
	};
}
