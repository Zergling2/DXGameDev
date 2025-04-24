#pragma once

#include <ZergEngine\Common\EngineHelper.h>

namespace ze
{
	class IScene;

	using SceneFactory = std::unique_ptr<IScene>(*)();

	#define ZE_DECLARE_SCENE(ClassName)\
	public:\
		static std::unique_ptr<IScene> SF();\
	private:\
		class ClassName##Register\
		{\
		private:\
			ClassName##Register();\
		private:\
			static ClassName##Register s_autoRegister;\
		}
	
	
	#define ZE_IMPLEMENT_SCENE(ClassName)\
	std::unique_ptr<IScene> ClassName::SF() { return std::make_unique<ClassName>(); }\
	ClassName::ClassName##Register::ClassName##Register()\
	{\
		SceneTable::AddItem(#ClassName, ClassName::SF);\
	}\
	ClassName::ClassName##Register ClassName::ClassName##Register::s_autoRegister;


	class SceneTable
	{
		friend class SceneManagerImpl;
	public:
		static void AddItem(PCSTR sceneName, SceneFactory factory);
		static SceneFactory GetItem(PCSTR sceneName);
	private:
		static std::map<PCSTR, SceneFactory, MultiByteStringComparator>* s_pSceneTable;
	};
}
