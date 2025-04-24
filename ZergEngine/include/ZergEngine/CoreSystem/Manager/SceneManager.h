#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\SlimRWLock.h>

namespace ze
{
	class IScene;
	class GameObject;

	class AsyncOperation
	{
	public:
	private:
	};

	class AsyncOperationHandle
	{
	public:
		AsyncOperationHandle() = default;
		~AsyncOperationHandle() = default;
	private:
	};

	class SceneManagerImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(SceneManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void Update(float* pLoopTime);
		std::unique_ptr<IScene> CreateScene(PCSTR sceneName);
	public:
		bool LoadScene(PCSTR sceneName);
		AsyncOperationHandle LoadSceneAsync(PCSTR sceneName);
	private:
		SlimRWLock m_lock;
		std::unique_ptr<IScene> m_upCurrentScene;
		std::unique_ptr<IScene> m_upNextScene;
	};

	extern SceneManagerImpl SceneManager;
}
