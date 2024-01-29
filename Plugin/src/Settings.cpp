#include "Settings.h"

namespace Settings
{
	void Main::Load() noexcept
	{
		static std::once_flag ConfigInit;
		std::call_once(ConfigInit, [&]() {
			config.Bind<0, 2>(cameraType, 0);
			config.Bind(enableMovement, false);
			config.Bind(disablePOVChange, true);
		});

		config.Load();

		INFO("Config loaded"sv)
	}
}