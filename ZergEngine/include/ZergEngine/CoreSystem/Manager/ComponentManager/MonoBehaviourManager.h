#pragma once

#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	class MonoBehaviour;

	class MonoBehaviourManager : public IComponentManager
	{
		friend class Runtime;
		friend class MonoBehaviour;
	public:
		static MonoBehaviourManager* GetInstance() { return s_pInstance; }
	private:
		MonoBehaviourManager();
		virtual ~MonoBehaviourManager() = default;

		static void CreateInstance();
		static void DestroyInstance();

		void RequestEnable(MonoBehaviour* pMonoBehaviour);
		void RequestDisable(MonoBehaviour* pMonoBehaviour);

		virtual void Deploy(IComponent* pComponent) override;
		void AddToStartQueue(MonoBehaviour* pMonoBehaviour);
		void AwakeDeployedComponents();
		void CallStart();
		void FixedUpdateScripts();
		void UpdateScripts();
		void LateUpdateScripts();

		virtual void RemoveDestroyedComponents() override;
	protected:
		static MonoBehaviourManager* s_pInstance;

		std::vector<MonoBehaviour*> m_awakeQueue;
		std::vector<MonoBehaviour*> m_startQueue;
	};
}
