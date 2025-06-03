package("argus")
    set_homepage("https://github.com/lucocozz/argus")
    set_description("Modern C library for command-line argument parsing with an elegant, declarative API")
    set_license("MIT")

    add_urls("https://github.com/lucocozz/argus/archive/refs/tags/v$(version).tar.gz")
    add_versions("0.1.0", "HASH_TO_REPLACE")

    add_configs("regex", {description = "Enable regex validation support using PCRE2", default = true, type = "boolean"})

    on_install(function (package)
        local configs = {"-Dregex=" .. (package:config("regex") and "true" or "false")}
        import("package.tools.meson").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cfuncs("argus_init", {includes = "argus.h"}))
    end)
