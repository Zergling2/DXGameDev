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

	class SceneManager : public ISubsystem
	{
		friend class Runtime;
		ZE_DECLARE_SINGLETON(SceneManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void Update(float* pLoopTime);
		std::unique_ptr<IScene> CreateScene(PCWSTR sceneName);
	public:
		bool LoadScene(PCWSTR sceneName);
		AsyncOperationHandle LoadSceneAsync(PCWSTR sceneName);
	private:
		SlimRWLock m_lock;
		std::unique_ptr<IScene> m_upCurrentScene;
		std::unique_ptr<IScene> m_upNextScene;
	};
}
