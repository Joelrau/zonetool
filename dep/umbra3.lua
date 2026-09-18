-- Umbra 3.3.13 occlusion culling SDK (runtime + optimizer), as published in the
-- cohaereo/Deimos repository (crates/umbra3/umbra3-sys/umbra-source). Only the
-- out-of-process tome generator links it, so it is built x64 regardless of the
-- workspace default; ZoneTool itself never links it.
umbra3 = {}

function umbra3:include()
	local folder = DependencyFolder()
	includedirs {
		path.join(folder, "umbra3/interface"),
		path.join(folder, "umbra3/interface/runtime"),
		path.join(folder, "umbra3/interface/optimizer"),
		path.join(folder, "umbra3/source"),
		path.join(folder, "umbra3/source/common"),
		path.join(folder, "umbra3/source/runtime"),
		path.join(folder, "umbra3/source/optimizer"),
		path.join(folder, "umbra3/source/standard"),
	}
end

function umbra3:link()
	self:include()
	links {
		"umbra3"
	}
end

function umbra3:project()
	local folder = DependencyFolder()

	project "umbra3"
		location "%{wks.location}/dep"
		kind "StaticLib"
		language "C++"
		architecture "x86_64"
		characterset "MBCS"

		-- The SDK predates C++20's rewritten comparison operators (C2666 in
		-- umbraSubdivisionTree.hpp under /std:c++latest) and assumes an ANSI
		-- Win32 API.
		removebuildoptions { "/std:c++latest" }
		cppdialect "C++14"

		-- The optimizer is unusably slow unoptimized and nothing else links it,
		-- so it is built optimized with the release CRT in every configuration.
		optimize "Speed"
		runtime "Release"
		removedefines { "_DEBUG", "DEBUG" }
		defines { "NDEBUG" }

		-- umbraMemory.cpp is the SDK's own heap; umbra_stub_allocator.cpp is the
		-- malloc-backed replacement the source ships with, and both define the
		-- same symbols. The per-platform thread/process files live under windows/.
		files {
			path.join(folder, "umbra3/interface/**.hpp"),
			path.join(folder, "umbra3/source/common/*.cpp"),
			path.join(folder, "umbra3/source/common/*.hpp"),
			path.join(folder, "umbra3/source/common/windows/*.cpp"),
			path.join(folder, "umbra3/source/common/windows/*.inl"),
			path.join(folder, "umbra3/source/standard/*.cpp"),
			path.join(folder, "umbra3/source/standard/*.hpp"),
			path.join(folder, "umbra3/source/runtime/*.cpp"),
			path.join(folder, "umbra3/source/runtime/*.hpp"),
			path.join(folder, "umbra3/source/optimizer/*.cpp"),
			path.join(folder, "umbra3/source/optimizer/*.hpp"),
		}

		removefiles {
			path.join(folder, "umbra3/source/common/umbraMemory.cpp"),
		}

		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_WARNINGS",
			-- disables the optimizer's license check (umbraLicense.cpp)
			"UMBRA_UNLOCKED",
			-- exit portals on every cell face, see umbraTomeGenerator.cpp buildCellGraph
			"UMBRA_IW7_CELL_EXIT_PORTALS",
		}

		removedefines { "CPU_32BIT" }

		self:include()

		-- not our code
		warnings "off"
end
