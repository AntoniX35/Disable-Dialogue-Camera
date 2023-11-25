#pragma once
#define CONFIG_ENTRY ""
#include "DKUtil/Config.hpp"

namespace Settings
{
    using namespace DKUtil::Alias;

    class Main : public DKUtil::model::Singleton<Main>
    {
    public:
		Integer cameraType{ "iCameraType", "Main" };
		Boolean enableMovement{ "bEnableMovement", "Main" };

        void Load() noexcept;

    private:
		IniConfig config = COMPILE_PROXY("DisableDialogueCamera.ini"sv);
    };
}