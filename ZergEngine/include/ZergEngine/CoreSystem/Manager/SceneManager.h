#pragma once

#include <ZergEngine\CoreSystem\SlimRWLock.h>

namespace ze
{
	class IScene;

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

	class SceneManager
	{
		friend class Runtime;
	public:
		static SceneManager* GetInstance() { return s_pInstance; }
	private:
		SceneManager();
		~SceneManager();

		static void CreateInstance();
		static void DestroyInstance();

		void Init(PCWSTR startScene);
		void Shutdown();

		IScene* CreateScene(PCWSTR sceneName);
		IScene* PopNextScene();
	public:
		bool LoadScene(PCWSTR sceneName);
		AsyncOperationHandle LoadSceneAsync(PCWSTR sceneName);
	private:
		static SceneManager* s_pInstance;
		SlimRWLock m_lock;
		IScene* m_pNextScene;
	};
}
