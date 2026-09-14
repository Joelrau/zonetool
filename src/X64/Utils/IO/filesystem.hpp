#pragma once

namespace zonetool
{
	namespace filesystem
	{
		static std::string fastfile;

		class file
		{
		public:
			file(const std::string& filepath);
			file();
			~file();
			FILE* get_fp();
			bool exists();
			errno_t open(std::string mode = "wb", bool use_path = true, bool is_zone = false);
			size_t write_string(const std::string& str);
			size_t write_string(const char* str);
			size_t write(const std::string& str);
			size_t write(const void* buffer, size_t size, size_t count);
			template <typename T> size_t write(const T* val, size_t size = sizeof(T), size_t count = 1)
			{
				return this->write(reinterpret_cast<const void*>(val), size, count);
			}
			size_t read_string(std::string* str);
			size_t read(void* buffer, size_t size, size_t count);
			template <typename T> size_t read(T* val, size_t size = sizeof(T), size_t count = 1)
			{
				return this->read(reinterpret_cast<void*>(val), size, count);
			}
			template <typename T> size_t read(const T* val, size_t size = sizeof(T), size_t count = 1)
			{
				return this->read(const_cast<T*>(val), size, count);
			}
			int close();

			bool create_path();

			std::size_t size();
			std::vector<uint8_t> read_bytes(std::size_t size);
		private:
			FILE* fp = {};

			std::filesystem::path filepath;
			std::string parent_path;
			std::string filename;
		};

		void set_fastfile(const std::string& ff);
		const std::string& get_fastfile();
		std::string get_zone_path(const std::string& name = "");
		std::string get_file_path(const std::string& name);
		std::string get_dump_path();
		bool create_directory(const std::string& name);

		// ---- zone source (.csv) -----------------------------------------------------------
		//
		// The linkers write the csv from the source game's own asset name, but a dumper is free
		// to write the asset out under a different one - IW7 materials get their prefix rewritten
		// to match the mapped techset, so mc/mtl_metal_pail lands in materials\mo\. The csv has to
		// name the file that actually exists or the IW7 linker resolves nothing.
		//
		// A dumper cannot fix its own line as it is written: the linker emits it just before the
		// asset is dumped, so the new name does not exist yet. So the lines are buffered, dumpers
		// register renames as they go, and the substitution happens at close, by which point every
		// asset in the zone - referenced ones included - has been through its dumper.
		void csv_reset();
		void csv_buffer_line(const std::string& type, const std::string& name);
		void csv_register_rename(const std::string& type, const std::string& from,
			const std::string& to);
		std::vector<std::string> csv_take_lines();
	}
}