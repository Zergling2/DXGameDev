#pragma once

#include <ZergEngine\Common\EngineHelper.h>

namespace ze
{
	class IScene;

	using SceneFactory = IScene*(*)();

	#define ZE_DECLARE_SCENE(ClassName)\
	public:\
		static IScene* SF();\
	private:\
		class ClassName##Register\
		{\
		private:\
			ClassName##Register();\
		private:\
			static ClassName##Register s_autoRegister;\
		}
	
	
	#define ZE_IMPLEMENT_SCENE(ClassName)\
	IScene* ClassName::SF() { return new ClassName(); }\
	ClassName::ClassName##Register::ClassName##Register()\
	{\
		SceneTable::AddItem(L#ClassName, ClassName::SF);\
	}\
	ClassName::ClassName##Register ClassName::ClassName##Register::s_autoRegister;


	class SceneTable
	{
		friend class SceneManager;
	public:
		static void AddItem(PCWSTR sceneName, SceneFactory factory);
		static SceneFactory GetItem(PCWSTR sceneName);
	private:
		static std::map<PCWSTR, SceneFactory, WideCharStringComparator>* s_pSceneTable;
	};
}
