#include "stdafx.hpp"
#include "havok.hpp"

namespace ZoneTool::IW7
{
	namespace havok
	{
		constexpr auto havok_version = "hk_2014.2.5-r1";

		static const char* get_version()
		{
			return havok_version;
		}

		namespace xml
		{

		}

		namespace binary
		{
			constexpr auto hk_magic1 = 0x57E0E057;
			constexpr auto hk_magic2 = 0x10C0C010;

			struct hkxHeaderData
			{
				unsigned int magic1; // 0x57E0E057
				unsigned int magic2; // 0x10C0C010
				unsigned int userTag;
				unsigned int version;
				unsigned char pointerSize;
				bool littleEndian;
				bool reuseBaseClassPadding;
				bool emptyBaseClassOptimization;
				int numSections;
				int contentsSectionIndex;
				int contentsSectionOffset;
				int contentsClassNameSectionIndex;
				int contentsClassNameSectionOffset;
				char contentsVersion[16];
				unsigned int flags;
				short maxpredicate;
				short predicateArraySizePlusPadding;
			};

			bool validate_header(hkxHeaderData* header)
			{
				if (!header)
				{
					ZONETOOL_ERROR("Pointer is null");
					return false;
				}

				if (header->magic1 != hk_magic1 || header->magic2 != hk_magic2)
				{
					ZONETOOL_ERROR("Missing packfile magic header. Is this from a binary file?");
					return false;
				}

				if (header->pointerSize != sizeof(std::uintptr_t))
				{
					ZONETOOL_ERROR("Trying to process a binary file with a different pointer size than this platform.");
					return false;
				}

				if (header->littleEndian != true)
				{
					ZONETOOL_ERROR("Trying to process a binary file with a different endian than this platform.");
					return false;
				}

				if (header->reuseBaseClassPadding != false)
				{
					ZONETOOL_ERROR("Trying to process a binary file with a different padding optimization than this platform.");
					return false;
				}

				if (header->emptyBaseClassOptimization != true)
				{
					ZONETOOL_ERROR("Trying to process a binary file with a different empty base class optimization than this platform.");
					return false;
				}

				if (header->contentsVersion[0] != -1)
				{
					if (!strcmp(header->contentsVersion, get_version()))
					{
						return true;
					}
					else
					{
						ZONETOOL_ERROR("Packfile contents are not up to date");
						return false;
					}
				}

				ZONETOOL_ERROR("Packfile file format is too old");
				return false;
			}

			void dump_havok_data(std::string path, char* data, unsigned int size)
			{
				if (!path.ends_with(havok_file_ext))
				{
					path.append(binary::havok_file_ext);
				}

				if (!data || !size)
				{
					return;
				}

				if (!validate_header(reinterpret_cast<binary::hkxHeaderData*>(data)))
				{
					__debugbreak();
				}

				auto file = filesystem::file(path);
				file.open("wb");
				file.write(data, size, 1);
				file.close();
			}
		}
	}
}