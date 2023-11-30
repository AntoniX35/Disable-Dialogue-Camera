#include "main.h"

namespace DisableDialogueCamera
{
	std::int32_t camera;
	bool firstPerson = false;
	bool thirdPerson = false;

	REL::Relocation<std::uintptr_t> Call{ REL::ID(179039) };
	
	REL::Relocation<std::uintptr_t> NoBlock1{ REL::ID(137819), 0x5F};
	REL::Relocation<std::uintptr_t> NoBlock2{ REL::ID(137819), 0x70};
	
	REL::Relocation<std::uintptr_t> Addr{ REL::ID(817115) };
	
	REL::Relocation<std::uintptr_t> AlwaysShow{ REL::ID(145943), 0x2E7};
	
	
	RE::CameraState CameraState()
	{
		return *SFSE::stl::adjust_pointer<RE::CameraState>(RE::PlayerCamera::GetSingleton()->currentState, 0x50);
	}

	bool FirstPerson()
	{
		return CameraState() == RE::CameraState::kFirstPerson;
	}

	void Install()
	{
	   	const auto settings = Settings::Main::GetSingleton();
		
		camera = *settings->cameraType;

		REL::Relocation<std::uintptr_t> DialogueCamera1{ REL::ID(100146), -0xCB};
		REL::safe_write(DialogueCamera1.address(), &REL::NOP6, sizeof(REL::NOP6));

		REL::Relocation<std::uintptr_t> DialogueCamera2{ REL::ID(153784), 0x2A};
		constexpr std::uint8_t jmp[] = { 0xE9, 0x8D, 0x07, 0x00, 0x00, 0x90, 0x90 };
		REL::safe_write(DialogueCamera2.address(), &jmp, sizeof(jmp));
		
		REL::safe_write(AlwaysShow.address(), &REL::JMP8, sizeof(REL::JMP8));
		
		RE::UI::GetSingleton()->RegisterSink(EventHandler::GetSingleton());

		if (*settings->enableMovement) {
			REL::safe_write(NoBlock2.address(), &REL::NOP5, sizeof(REL::NOP5));
			{
				struct Gamepad_Code : Xbyak::CodeGenerator
				{
					Gamepad_Code()
					{
						Xbyak::Label retnLabel;
						Xbyak::Label callLabel;
						Xbyak::Label addrLabel;
					
						mov(rax, ptr[rip + addrLabel]);
						cmp(byte[rax], 1);
						je("KeyBoard");
						call(ptr[rip + callLabel]);
					
						L("KeyBoard");
						jmp(ptr[rip + retnLabel]);					
					
						L(addrLabel);
						dq(Addr.address());
					
						L(callLabel);
						dq(Call.address());					

						L(retnLabel);
						dq(NoBlock1.address() + 0x5);
					}
				};

				Gamepad_Code code;
				code.ready();

				auto& trampoline = SFSE::GetTrampoline();
				trampoline.write_branch<5>(NoBlock1.address(), trampoline.allocate(code));
			}
		}

		INFO("Installed");
	}
		
	RE::BSEventNotifyControl EventHandler::ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		{
			if (a_event.menuName == "DialogueMenu") 
			{
				if (a_event.opening)
				{
					if (FirstPerson()) 
					{
						if (camera == 2)
						{
							RE::PlayerCamera::GetSingleton()->ForceThirdPerson();
							firstPerson = true;
						}
					}
					else
					{
						if (camera == 1)
						{
							RE::PlayerCamera::GetSingleton()->ForceFirstPerson();
							thirdPerson = true;
						}
					}
				}
				else 
				{					
					if (FirstPerson())
					{
						if (camera == 1 && thirdPerson)
						{
							RE::PlayerCamera::GetSingleton()->ForceThirdPerson();
							thirdPerson = false;
						}
					}
					else
					{
						if (camera == 2 && firstPerson)
						{
							RE::PlayerCamera::GetSingleton()->ForceFirstPerson();
							firstPerson = false;
						}
					}
				}			
			}

			return RE::BSEventNotifyControl::kContinue;
		}
	}
}

namespace
{
	void MessageCallback(SFSE::MessagingInterface::Message* a_msg) noexcept
	{
		switch (a_msg->type) {
		case SFSE::MessagingInterface::kPostLoad:
			{
				Settings::Main::GetSingleton()->Load();
				DisableDialogueCamera::Install();
			}
			break;
		default:
			break;
		}
	}
}

DLLEXPORT bool SFSEAPI SFSEPlugin_Load(const SFSE::LoadInterface* a_sfse)
{
#ifndef NDEBUG
	MessageBoxA(NULL, "Loaded. You can now attach the debugger or continue execution.", Plugin::NAME.data(), NULL);
#endif

	SFSE::Init(a_sfse, false);
	DKUtil::Logger::Init(Plugin::NAME, std::to_string(Plugin::Version));
	INFO("{} v{} loaded", Plugin::NAME, Plugin::Version);

	// do stuff
	// this allocates 1024 bytes for development builds, you can
	// adjust the value accordingly with the log result for release builds
	SFSE::AllocTrampoline(1 << 10);

	SFSE::GetMessagingInterface()->RegisterListener(MessageCallback);

	return true;
}
