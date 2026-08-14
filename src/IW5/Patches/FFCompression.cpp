#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		std::int32_t FFCompression::ff_version = 1;

		std::int32_t FFCompression::z_inflateInit(const char* version, db_z_stream_s* strm, int stream_size)
		{
			// Allocate zstd context
			strm->state = reinterpret_cast<db_zstd_context_s*>(malloc(sizeof(db_zstd_context_s)));
			memset(strm->state, 0, sizeof(db_zstd_context_s));

			// Create pointers needed for decompression
			strm->state->DCtx = ZSTD_createDCtx();
			strm->state->DStream = ZSTD_createDStream();

			// return ZLIB ok state
			return Z_OK;
		}

		__declspec(naked) void FFCompression::db_inflateInit()
		{
			__asm
			{
				cmp ff_version, 2000;
				jl jmp_back;

				// use zstd
				push 52;
				push esi;
				push eax;
				call FFCompression::z_inflateInit;

				add esp, 12;
				retn;

				// use zlib
				jmp_back:
				push edi
				xor edi, edi
				cmp eax, edi

				push 0x5DAF25;
				retn;
			}
		}

		std::int32_t FFCompression::z_inflate(db_z_stream_s* strm)
		{
			// if there are no bytes available in stream..
			if (strm->avail_in <= 0)
			{
				return Z_OK;
			}

			ZSTD_inBuffer inBuf;
			ZSTD_outBuffer outBuf;

			inBuf.src = strm->next_in;
			inBuf.size = strm->avail_in;
			inBuf.pos = 0;

			outBuf.dst = strm->next_out;
			outBuf.size = strm->avail_out;
			outBuf.pos = 0;

			auto code = ZSTD_decompressStream(strm->state->DStream, &outBuf, &inBuf);
			if (ZSTD_isError(code))
			{
				ZONETOOL_ERROR("An error occured while decompressing zone: %s", ZSTD_getErrorName(code));
				return Z_STREAM_ERROR;
			}

			strm->next_out += outBuf.pos;
			strm->total_out += outBuf.pos;
			strm->avail_out -= outBuf.pos;

			strm->next_in += inBuf.pos;
			strm->total_in += inBuf.pos;
			strm->avail_in -= inBuf.pos;

			return Z_OK;
		}

		__declspec(naked) void FFCompression::db_inflate()
		{
			__asm
			{
				cmp ff_version, 2000;
				jl jmp_back;

				// use zstd
				push eax;
				call FFCompression::z_inflate;

				add esp, 4;
				retn;

				// use zlib
				jmp_back:
				push ebp;
				mov ebp, esp;
				and esp, 0x0FFFFFFF8;

				push 0x5DAFE6;
				retn;
			}
		}

		std::int32_t FFCompression::z_inflateEnd(db_z_stream_s* strm)
		{
			// free decompression stream
			ZSTD_freeDCtx(strm->state->DCtx);
			ZSTD_freeDStream(strm->state->DStream);
			free(strm->state);

			// return ZLIB ok state
			return Z_OK;
		}

		__declspec(naked) void FFCompression::db_inflateEnd()
		{
			__asm
			{
				cmp ff_version, 2000;
				jl jmp_back;

				// use zstd
				push eax;
				call FFCompression::z_inflateEnd;

				add esp, 4;
				retn;

				// use zlib
				jmp_back:
				push edi;
				mov edi, eax;

				test edi, edi;
				jz error;

				push 0x5DAED7;
				retn;

				error:
				push 0x5DAF0D;
				retn;
			}
		}

		void FFCompression::SetFastfileVersion(std::int32_t version)
		{
			ff_version = version;
		}

		__declspec(naked) void FFCompression::DB_ReadXFileVersion()
		{
			__asm
			{
				cmp eax, 1;
				je ok;

				cmp eax, 1000;
				jae ok;

				push 0x436AD7;
				retn;

				ok:
				push eax;
				call FFCompression::SetFastfileVersion;
				add esp, 4;

				push 0x436AFA;
				retn;
			}
		}

		__declspec(naked) void FFCompression::XModelSizeStub()
		{
			__asm
			{
				cmp ff_version, 2000;
				jl originalSize;

				add DWORD PTR ds : 0x16634fc, 372;
				jmp goBack;

				originalSize:
				add DWORD PTR ds : 0x16634fc, 308;

				goBack:
				push 0x0043C50E;
				retn;
			}
		}

		__declspec(naked) void FFCompression::XModelSizeStub2()
		{
			__asm
			{
				cmp ff_version, 2000;
				jl originalSize;

				mov ecx, 372;
				jmp goBack;

				originalSize:
				mov ecx, 308;

				goBack:
				push 0x0043C4EE;
				retn;
			}
		}

		static char keyBuffer[64];

		__declspec(naked) void FFCompression::ReadEncryptedKey()
		{
			static std::uintptr_t DB_ReadXFileUncompressed = 0x00436EA0;

			__asm
			{
				// read original data
				call DB_ReadXFileUncompressed;

				// check if fastfile requires decryption
				cmp ff_version, 2000;
				jl goBack;

				// save registers
				pushad;

				// read encrypted data
				mov ecx, 64;
				lea eax, keyBuffer;
				call DB_ReadXFileUncompressed;

				// restore registers
				popad;

				goBack:
				// go back
				push 0x00436B24;
				retn;
			}
		}

		__declspec(naked) void FFCompression::MaterialSizeStub()
		{
			__asm
			{
				cmp ff_version, 2000;
				jl originalSize;

				add DWORD PTR ds : 0x16634fc, 0x6C;
				jmp goBack;

				originalSize:
				add DWORD PTR ds : 0x16634fc, 0x68;

				goBack:
				push 0x0043AB1B;
				retn;
			}
		}

		__declspec(naked) void FFCompression::MaterialSizeStub2()
		{
			__asm
			{
				cmp ff_version, 2000;
				jl originalSize;

				mov ecx, 0x6C;
				jmp goBack;

				originalSize:
				mov ecx, 0x68;

				goBack:
				push 0x0043AB03;
				retn;
			}
		}

		__declspec(naked) void FFCompression::AlignmentHook()
		{
			__asm
			{
				cmp ff_version, 2000;
				jl originalSize;

				add eax, 7;
				jmp goBack;

				originalSize:
				add eax, 3;

				goBack:
				and eax, 0FFFFFFFCh;

				push 0x00436C4D;
				retn;
			}
		}

		__declspec(naked) void FFCompression::AlignmentHook2()
		{
			__asm
			{
				cmp ff_version, 2000;
				jl originalSize;

				add eax, 8;
				jmp goBack;

				originalSize:
				add eax, 3;

				goBack:
				and eax, 0FFFFFFFCh;

				push 0x0043FE69;
				retn;
			}
		}

		template <typename T>
		static void decrypt_data(T* _data, std::size_t _size)
		{
			auto fastfile = static_cast<std::string>(*reinterpret_cast<const char**>(0x1294A00) + 4);

			auto encryptionKey = static_cast<std::string>(fastfile + ": This fastfile is property of the Plutonium Project."
				);
			auto dataptr = reinterpret_cast<char*>(_data);

			auto keyPos = 0;
			for (int rounds = 3; rounds >= 0; rounds--)
			{
				for (auto i = 0u; i < _size; i++)
				{
					if (keyPos >= encryptionKey.size())
					{
						keyPos = 0;
					}

					dataptr[i] = dataptr[i] ^ rounds;
					dataptr[i] = dataptr[i] ^ encryptionKey[keyPos];
					dataptr[i] = 0xFF - dataptr[i];
					keyPos++;
				}
			}
		}

		void FFCompression::DecryptStream(char* data, std::size_t size)
		{
			if (ff_version >= 2000)
			{
				ZONETOOL_INFO("Decrypting data at ptr 0x%08X, size is %u", data, size);
				decrypt_data(data, size);
			}
		}

		__declspec(naked) void FFCompression::EncryptedLoadStream()
		{
			static std::uintptr_t load_stream = 0x00436F20;

			__asm
			{
				pushad;
				push ecx;
				push eax;
				call load_stream;
				call FFCompression::DecryptStream;
				add esp, 8;
				popad;

				retn;
			}
		}

		FFCompression::FFCompression()
		{
			// Allow loading of unsigned fastfiles
			Memory(0x436B3D).nop(2);

			// Compression hooks
			Memory(0x5DAF20).jump(db_inflateInit);
			Memory(0x5DAED0).jump(db_inflateEnd);
			Memory(0x5DAFE0).jump(db_inflate);

			// Fastfile version hook
			Memory(0x436AD2).jump(DB_ReadXFileVersion);

			// encrypted load stream hooks
			Memory(0x00440CF4).call(EncryptedLoadStream); // col_map_mp
			Memory(0x00441AEB).call(EncryptedLoadStream); // com_map
			Memory(0x0044075B).call(EncryptedLoadStream); // map_ents
			Memory(0x0043F38B).call(EncryptedLoadStream); // fx_map
			Memory(0x0044C244).call(EncryptedLoadStream); // gfx_map
			Memory(0x00446E6E).call(EncryptedLoadStream); // weapon
			Memory(0x0043A92E).call(EncryptedLoadStream); // techset

			// Encryption hooks
			// Memory(0x00436B1F).Jump(FFCompression::ReadEncryptedKey);
		}

		FFCompression::~FFCompression()
		{
		}
	}
}
