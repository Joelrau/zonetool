S1 = {}

function S1:include()
    includedirs {
        path.join(ProjectFolder(), "S1")
    }
end

function S1:link()
    self:include()
	links {
		"S1"
	}
end

function S1:project()
    local folder = ProjectFolder();

    project "S1"
        kind "StaticLib"
        language "C++"
        
        pchheader "stdafx.hpp"
        pchsource(path.join(folder, "S1/stdafx.cpp"))

        files {
            path.join(folder, "S1/**.h"),
            path.join(folder, "S1/**.hpp"),
            path.join(folder, "S1/**.cpp")
        }

        self:include()
        ZoneUtils:include()

        zstd:include()
        zlib:include()

        X64:include()
end
