#pragma once
#include "DKUtil/Config.hpp"
#include "Settings.h"
bool newver;
template <class T>
class ISingleton
{
public:
    static T* GetSingleton()
    {
        static T singleton;
        return std::addressof(singleton);
    }

protected:
    ISingleton()  = default;
    ~ISingleton() = default;

    ISingleton(const ISingleton&)            = delete;
    ISingleton(ISingleton&&)                 = delete;
    ISingleton& operator=(const ISingleton&) = delete;
    ISingleton& operator=(ISingleton&&)      = delete;
};

namespace DisableDialogueCamera
{
	class EventHandler :
		public ISingleton<EventHandler>,
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;
	};

	void Install();
}