workspace "PewParser"
    configurations { "Release", "Debug" }
    platforms { "x86", "x64" }
    defaultplatform "x64"

    -- Build directory
    builddir = "%{wks.location}/build/%{cfg.buildcfg}-%{cfg.platform}"

project "PewParser"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    
    targetdir (builddir .. "/bin/")
    objdir (builddir .. "/obj/")

    files {
        "include/PewParser/PewParser.h",
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "include",
        "src"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter "system:windows"
        systemversion "latest"
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "system:linux"
        buildoptions { "-Wno-format-security" }

    filter "platforms:x64"
        architecture "x64"

    filter "platforms:x86"
        architecture "x86"
