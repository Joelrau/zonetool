#pragma once

#define SELECT(mp, zm) (strcmp(reinterpret_cast<char*>(0xBC8D34), "COD_T6_S MP") ? mp : zm)

#include <ZoneUtils.hpp>
#include "Structs.hpp"
#include "Functions.hpp"
#include "Patches/AssetHandler.hpp"

#include "X64/X64.hpp"
#include "X64/Utils/IO/filesystem.hpp"
#include "X64/Utils/IO/assetmanager.hpp"

#include "H1/Structs.hpp"

using namespace zonetool;

#include "Json.hpp"
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

#include "Assets/RawFile.hpp"
#include "Assets/StringTable.hpp"

namespace ZoneTool
{
	namespace T6
	{
		class params
		{
		public:
			params();

			int size() const;
			const char* get(int index) const;
			std::string join(int index) const;

			const char* operator[](const int index) const
			{
				return this->get(index);
			}

		private:
			int nesting_;
		};

		void init();
	}
}
