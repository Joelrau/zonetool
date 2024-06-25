// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include <Dbghelp.h>

// include zonetool linkers
#include <IW3/IW3.hpp>
#include <IW4/IW4.hpp>
#include <IW5/IW5.hpp>
#include <T6/T6.hpp>
#include <H1/H1.hpp>
#include <S1/S1.hpp>

#pragma comment(lib, "Dbghelp")

std::string currentzone;

zonetool::dump_target zonetool::dumping_target = zonetool::dump_target::h1;

namespace ZoneTool
{
	linker_mode currentlinkermode{ linker_mode::none };

	std::vector<std::shared_ptr<ILinker>> linkers;
	std::map<std::string, std::function<void(std::vector<std::string>)>> commands;

	void register_command(const std::string& name, std::function<void(std::vector<std::string>)> cb)
	{
		commands[name] = cb;
	}

	void execute_command(std::vector<std::string> args)
	{
		const auto itr = commands.find(args[0]);
		if (itr != commands.end())
		{
			itr->second(args);
		}
		else
		{
			ZONETOOL_ERROR("Unknown command \"%s\".", args[0].data());
		}
	}

	void command_thread()
	{
		while (true)
		{
			// Get console input
			std::string input;
			std::getline(std::cin, input);

			std::vector<std::string> args;

			// Load arguments into vector
			if (input.find(' ') != std::string::npos)
			{
				args = split(input, ' ');
			}
			else
			{
				args.push_back(input);
			}

			// Execute command
			execute_command(args);
		}
	}
	
	void build_zone(ILinker* linker, const std::string& fastfile)
	{
		
	}

	ILinker* current_linker;

	LONG NTAPI exception_handler(_EXCEPTION_POINTERS* info)
	{
		if (info->ExceptionRecord->ExceptionCode == STATUS_INTEGER_OVERFLOW ||
			info->ExceptionRecord->ExceptionCode == STATUS_FLOAT_OVERFLOW ||
			info->ExceptionRecord->ExceptionCode == 0x406D1388 || 
			info->ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT)
		{
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		
		if (ZONETOOL_VERSION == "0.0.0"s)
		{
			MessageBoxA(nullptr, va("An exception occured (0x%08X) and ZoneTool must be restarted to continue. However, ZoneTool has detected that you are using a custom DLL. If you want to submit an issue, try to reproduce the bug with the latest release of ZoneTool. The latest version can be found here: https://github.com/ZoneTool/zonetool/releases", info->ExceptionRecord->ExceptionCode).data(), "ZoneTool", MB_ICONERROR);
			std::exit(0);
		}
		
		std::filesystem::create_directories("zonetool/crashdumps");

		const auto exception_time = std::time(nullptr);
		const auto linker_name = (current_linker) ? current_linker->version() : "unknown";
		const auto file_name = va("zonetool/crashdumps/zonetool-exception-%s-%s-%llu.dmp", linker_name, ZONETOOL_VERSION, exception_time);
		
		DWORD dump_type = MiniDumpIgnoreInaccessibleMemory;
		dump_type |= MiniDumpWithHandleData;
		dump_type |= MiniDumpScanMemory;
		dump_type |= MiniDumpWithProcessThreadData;
		dump_type |= MiniDumpWithFullMemoryInfo;
		dump_type |= MiniDumpWithThreadInfo;
		dump_type |= MiniDumpWithCodeSegs;
		dump_type |= MiniDumpWithDataSegs;
		
		const DWORD file_share = FILE_SHARE_READ | FILE_SHARE_WRITE;
		const HANDLE file_handle = CreateFileA(file_name.data(), GENERIC_WRITE | GENERIC_READ, file_share, nullptr, (file_share & FILE_SHARE_WRITE) > 0 ? OPEN_ALWAYS : OPEN_EXISTING, NULL, nullptr);
		MINIDUMP_EXCEPTION_INFORMATION ex = { GetCurrentThreadId(), info, FALSE };
		if (!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle, static_cast<MINIDUMP_TYPE>(dump_type), &ex, nullptr, nullptr))
		{
			
		}

		const auto message = va("An exception occured and ZoneTool must be restarted to continue. If this keeps happening, create an issue on https://github.com/ZoneTool/zonetool with the crashdump attached. The crashdump can be found at: \"%s\".", file_name.data());
		MessageBoxA(nullptr, message.data(), "ZoneTool", MB_ICONERROR);
		std::exit(0);
	}
	
	void create_console()
	{
#ifdef USE_VMPROTECT
		VMProtectBeginUltra("CreateConsole");
#endif

		if (!IsDebuggerPresent())
		{
			// Catch exceptions
			//AddVectoredExceptionHandler(TRUE, exception_handler);
		}
		
		// Allocate console
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);

		// Set console name
		SetConsoleTitleA("ZoneTool");

		// Spawn command thread
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(command_thread), nullptr, 0, nullptr);

		// Commands
		register_command("quit"s, [](std::vector<std::string>)
		{
			ExitProcess(0);
		});
		register_command("buildzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: buildzone <zone>");
				return;
			}

			if (current_linker)
			{
				if(current_linker->supports_building())
				{
					build_zone(current_linker, args[1]);
				}
				else
				{
					ZONETOOL_ERROR("Current linker does not support zone building.");
				}
			}
		});
		register_command("loadzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: loadzone <zone>");
				return;
			}

			// Load zone
			if (current_linker)
			{
				current_linker->load_zone(args[1]);
			}
		});
		register_command("verifyzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: verifyzone <zone>");
				return;
			}

			// Load zone
			if (current_linker)
			{
				current_linker->verify_zone(args[1]);
			}
		});
		register_command("dumpzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: dumpzone <zone>, <target(optional)>");
				return;
			}

			auto target = zonetool::dump_target::h1;
			if (args.size() >= 3)
			{
				auto target_str = args[2];
				if (target_str == "h1")
				{
					target = zonetool::dump_target::h1;
				}
				else if (target_str == "iw6")
				{
					target = zonetool::dump_target::iw6;
				}
				else if (target_str == "iw7")
				{
					target = zonetool::dump_target::iw7;
				}
				else if (target_str == "s1")
				{
					target = zonetool::dump_target::s1;
				}				

				ZONETOOL_INFO("current dump target: %s (%d)", target_str.data(), target);
			}

			// Load zone
			if (current_linker)
			{
				current_linker->dump_zone(args[1], target);
			}
		});

#ifdef USE_VMPROTECT
		VMProtectEnd();
#endif
	}

	template <typename T>
	void register_linker()
	{
		linkers.push_back(std::make_shared<T>());
	}

	void branding(ILinker* linker)
	{
		ZONETOOL_INFO("ZoneTool initialization complete!");
		ZONETOOL_INFO("Welcome to ZoneTool v" ZONETOOL_VERSION " written by RektInator.");
		ZONETOOL_INFO("  \"No matter how hard or unlikely, if it's possible, it will be done.\"");
		ZONETOOL_INFO("Special thanks to: Laupetin, NTAuthority, momo5502, TheApadayo, localhost, X3RX35 & homura.");

		if (linker)
		{
			ZONETOOL_INFO("Initializing linker for game \"%s\"...\n", linker->version());
		}
		else
		{
			ZONETOOL_ERROR("No linker could be found for the current binary!\n");
		}
	}

#pragma warning(disable: 4244)
	std::vector<std::string> get_command_line_arguments()
	{
		LPWSTR* szArglist;
		int nArgs;

		szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

		std::vector<std::string> args;
		args.resize(nArgs);

		// convert all args to std::string
		for (int i = 0; i < nArgs; i++)
		{
			auto curArg = std::wstring(szArglist[i]);
			args[i] = std::string(curArg.begin(), curArg.end());
		}

		// return arguments
		return args;
	}

	void handle_params()
	{
		// Wait until the game is loaded
		Sleep(5000);

		// Execute command line commands?
		auto args = get_command_line_arguments();
		if (args.size() > 1)
		{
			for (auto i = 0u; i < args.size(); i++)
			{
				if (i < args.size() - 1)
				{
					if (args[i] == "-buildzone")
					{
						build_zone(current_linker, args[i + 1]);
						i++;
					}
					else if (args[i] == "-loadzone")
					{
						current_linker->load_zone(args[i + 1]);
						i++;
					}
					else if (args[i] == "-dumpzone")
					{
						std::string target_str = "";
						if (i + 2 < args.size())
						{
							target_str = args[i + 2];
						}

						auto target = zonetool::dump_target::h1;
						if (target_str == "h1")
						{
							target = zonetool::dump_target::h1;
						}
						else if (target_str == "iw6")
						{
							target = zonetool::dump_target::iw6;
						}
						else if (target_str == "iw7")
						{
							target = zonetool::dump_target::iw7;
						}

						std::string zone = args[i + 1];
						current_linker->dump_zone(zone, target);

						if (!target_str.empty()) i++;
						i++;
					}
				}
			}

			std::exit(0);
		}
	}

	bool is_custom_linker_present()
	{
		return std::filesystem::exists("linker.dll") && std::filesystem::is_regular_file("linker.dll");
	}

	void startup_default()
	{
		// Create stdout console
		create_console();

		// Register linkers
		register_linker<IW3::Linker>();
		register_linker<IW4::Linker>();
		register_linker<IW5::Linker>();

		// check if a custom linker is present in the current game directory
		if (is_custom_linker_present())
		{
			const auto linker_module = LoadLibraryA("linker.dll");

			if (linker_module != nullptr && linker_module != INVALID_HANDLE_VALUE)
			{
				const auto get_linker_func = GetProcAddress(linker_module, "GetLinker");

				if (get_linker_func != nullptr && get_linker_func != INVALID_HANDLE_VALUE)
				{
					current_linker = Function<ILinker * ()>(get_linker_func)();
				}
			}
		}

		if (!current_linker)
		{
			// Startup compatible linkers
			for (auto& linker : linkers)
			{
				linker->startup();

				if (linker->is_used())
				{
					current_linker = linker.get();
				}
			}
		}

		// Startup complete, show branding
		branding(current_linker);

		// handle startup commands
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(handle_params), nullptr, 0, nullptr);
	}

	bool is_t6()
	{
		return !strcmp(reinterpret_cast<char*>SELECT(0xC1A178, 0xBC8D34), "COD_T6_S MP");
	}

	void startup()
	{
		if (is_t6())
		{
			T6::init();
		}
		else
		{
			startup_default();
		}
	}
}
