toolchain("gcc-8.5.0")
    set_kind("standalone")
    set_sdkdir("/usr/local/gcc-8.5.0")
toolchain_end()

add_rules("mode.debug", "mode.release", "mode.releasedbg")
set_languages("c11", "cxx17")
set_toolchains("gcc-8.5.0")
-- 内部封装了 cl、clang、gcc 的异常选项：/EHsc、-fexceptions 
set_exceptions("cxx")

add_requires("conan::boost/1.69.0", {
    alias = "boost",
    configs = {
        shared = False,
        pic = true
    }
})
target("ftp-client")
    set_kind("binary")

    add_packages("boost")

    add_includedirs("src")
    add_files("src/cmdline/*.cpp",
              "src/ftp/*.cpp",
              "src/ftp/detail/*.cpp",
              "src/utils/*.cpp")
    add_syslinks("pthread", "stdc++fs")

    -- 自动生成 compile_commands.json 帮助代码补全跳转
    after_build(function (target)
        import("core.base.task")
        task.run("project", {kind = "compile_commands", outputdir = ".vscode"})
    end)
target_end()