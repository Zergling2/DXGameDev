#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class Rigidbody;

	class RigidbodyManager : public IComponentManager
	{
		friend class Runtime;
		friend class Rigidbody;
	public:
		static RigidbodyManager* GetInstance() { return s_pInstance; }
	private:
		RigidbodyManager() = default;
		virtual ~RigidbodyManager() = default;

		static void CreateInstance();
		static void DestroyInstance();

		void AddToKinematicBodyGroup(Rigidbody* pRigidbody);
		void RemoveFromKinematicBodyGroup(Rigidbody* pRigidbody);

		virtual void RemoveDestroyedComponents() override;

		void UpdateKinematicRigidbodyTransform() {}
	private:
		static RigidbodyManager* s_pInstance;
		std::vector<Rigidbody*> m_kinematicBodys;
	};
}
