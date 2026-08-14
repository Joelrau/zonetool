#pragma once

#include <Windows.h>

#include <initializer_list>
#include <functional>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <memory>
#include <vector>
#include <mutex>
#include <map>

#include "Json.hpp"
using Json = nlohmann::json;

#undef xor
#undef and

namespace ZoneTool
{
#pragma push(pack, 1)
	struct XZoneInfo
	{
		const char* zone;
		std::int32_t loadFlags;
		std::int32_t unloadFlags;
	};
#pragma push(pop)
}

namespace zonetool
{
	enum dump_target
	{
		iw7,
		iw6,
		h1,
		s1,
	};

	enum dump_source
	{
		source_none,
		iw3,
		iw4,
		iw5,
		t6,
	};

	extern dump_target dumping_target;
	extern dump_source dumping_source;
}

namespace ZoneTool
{
	namespace Shared
	{
		extern const char* SL_ConvertToString(std::uint16_t index);
		extern short SL_AllocString(const std::string& string);
	}
	using namespace Shared;
}

template <typename T1, typename T2>
std::size_t Difference(const T1& t1, const T2& t2)
{
	return std::uintptr_t(t1) - std::uintptr_t(t2);
}

template <typename ... Args>
std::string va(const std::string& format, Args ... args)
{
	size_t size = _snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	std::vector<char> buf;
	buf.resize(size);
	_snprintf(buf.data(), size, format.c_str(), args ...);
	return std::string(buf.data(), buf.data() + size - 1);
}

static std::string to_lower_copy(const std::string& str)
{
	std::string lowercase;
	lowercase.reserve(str.size()); // preallocate memory for efficiency
	for (char c : str)
	{
		lowercase += std::tolower(c);
	}
	return lowercase;
}

static std::string to_upper_copy(const std::string& str)
{
	std::string lowercase;
	lowercase.reserve(str.size()); // preallocate memory for efficiency
	for (char c : str)
	{
		lowercase += std::toupper(c);
	}
	return lowercase;
}

static bool is_numeric(const std::string& text)
{
	return std::to_string(atoi(text.data())) == text;
}

static std::vector<std::string> split(const std::string& rawInput, const std::vector<char>& delims)
{
	std::vector<std::string> strings;

	auto findFirstDelim = [](const std::string& input, const std::vector<char>& delims) -> std::pair<char, std::size_t>
	{
		auto firstDelim = 0;
		auto firstDelimIndex = static_cast<std::size_t>(-1);
		auto index = 0u;

		for (auto& delim : delims)
		{
			if ((index = input.find(delim)) != std::string::npos)
			{
				if (firstDelimIndex == -1 || index < firstDelimIndex)
				{
					firstDelim = delim;
					firstDelimIndex = index;
				}
			}
		}

		return { firstDelim, firstDelimIndex };
	};

	std::string input = rawInput;

	while (!input.empty())
	{
		auto splitDelim = findFirstDelim(input, delims);
		if (splitDelim.first != 0)
		{
			strings.push_back(input.substr(0, splitDelim.second));
			input = input.substr(splitDelim.second + 1);
		}
		else
		{
			break;
		}
	}

	strings.push_back(input);
	return strings;
}

static std::vector<std::string> split(const std::string& str, char delimiter)
{
	return split(str, std::vector<char>({ delimiter }));
}

#include "Zone/ZoneMemory.hpp"
#include "IAsset.hpp"
#include "IPatch.hpp"
#include "Utils/FileReader.hpp"
#include "Utils/FileSystem.hpp"
#include "Utils/Function.hpp"
#include "Utils/Memory.hpp"
#include "Linker.hpp"

#include "Compression.hpp"
#include "EntStrings.hpp"

#define REINTERPRET_CAST_SAFE(__TO__, __FROM__) \
	static_assert(sizeof(*__FROM__) == sizeof(*__TO__)); \
	__TO__ = reinterpret_cast<decltype(__TO__)>(__FROM__);

#define MAKE_STRING(__data__) #__data__

#define ASSET_SIZE(__size__) \
	void _assert_size() \
	{ \
		static_assert(sizeof(*this) == __size__, __FUNCTION__": Invalid struct size.\n"); \
	}

#define ZONETOOL_INFO(__FMT__,...) \
	printf("[ INFO ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

#define ZONETOOL_ERROR(__FMT__,...) \
	printf("[ ERROR ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

#define ZONETOOL_FATAL(__FMT__,...) \
	printf("[ FATAL ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__); \
	MessageBoxA(nullptr, &va("Oops! An unexpected error occured. Error was: " __FMT__ "\n\nZoneTool must be restarted to resolve the error. Last error code reported by windows: 0x%08X (%u)", __VA_ARGS__, GetLastError(), GetLastError())[0], nullptr, 0); \
	std::exit(0)

#define ZONETOOL_WARNING(__FMT__,...) \
	printf("[ WARNING ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

template <typename T>
static std::shared_ptr<T> RegisterPatch()
{
	return std::make_shared<T>();
}
