UmbraTomeGen = {}

-- x64 console tool that runs the Umbra 3 optimizer for the IW7 converter. ZoneTool
-- spawns it (see X64/Utils/Umbra/UmbraTome.cpp) rather than linking the SDK: the
-- converter is a 32-bit DLL inside the source game and cannot host it.
function UmbraTomeGen:project()
    local folder = ProjectFolder();

    project "umbra-tomegen"
        kind "ConsoleApp"
        language "C++"
        architecture "x86_64"
        characterset "MBCS"
        removebuildoptions { "/std:c++latest" }
        cppdialect "C++17"

        -- matches umbra3: optimized, release CRT, in every configuration
        optimize "Speed"
        runtime "Release"
        removedefines { "_DEBUG", "DEBUG" }
        defines { "NDEBUG" }

        files {
            path.join(folder, "UmbraTomeGen/**.hpp"),
            path.join(folder, "UmbraTomeGen/**.cpp"),
            path.join(folder, "X64/Utils/Umbra/UmbraScene.hpp"),
        }

        removedefines { "CPU_32BIT" }

        umbra3:link()
end
