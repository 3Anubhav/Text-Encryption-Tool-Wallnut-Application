project "TextEncryptionTool"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "Walnut/vendor/spdlog/include",
        "Walnut/vendor/glm",
        "Walnut/vendor/imgui",
        "Walnut/vendor/glfw/include",
        "Walnut/vendor/Glad/include",
        "Walnut/Walnut/src",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}"
    }

    links { "Walnut" }
