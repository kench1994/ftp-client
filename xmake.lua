-- toolchain("gcc-8.5.0")
--     set_kind("standalone")
--     set_sdkdir("/usr/local/gcc-8.5.0")
-- toolchain_end()

add_rules("mode.debug", "mode.release")
set_languages("c11", "cxx17")
--set_toolchains("gcc-8.5.0")
    
if is_os("windows") then
    set_config("arch", "x86")
    add_cxflags("/EHsc", {force = true})
    if is_mode("release") then
        add_cxflags("-MD", {force = true})
    else
        set_symbols("debug")
        set_config("vs_runtime", "MDd")
        add_cxflags("-MDd", {force = true})
    end
else
    set_config("arch", "x86_64")
    add_cxflags("-MD", {force = true})
end

if is_mode("release") then
    set_symbols("hidden")
    set_strip("all")
    add_cxflags("-fomit-frame-pointer")
    add_mxflags("-fomit-frame-pointer")
    set_optimize("faster")
else
    set_symbols("debug")
    set_optimize("none")
end


add_requires("conan::boost/1.69.0", {alias = "boost"})

target("ftp-client")
    set_kind("binary")
    add_packages("boost")


    add_includedirs("src")
    add_files("src/cmdline/*.cpp",
              "src/ftp/*.cpp",
              "src/ftp/detail/*.cpp",
              "src/utils/*.cpp")
    add_syslinks("pthread", "stdc++fs")
toolchain_end()
