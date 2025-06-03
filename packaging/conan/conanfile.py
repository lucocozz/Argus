from conan import ConanFile
from conan.tools.meson import Meson, MesonToolchain
from conan.tools.files import get, copy
from conan.tools.layout import basic_layout
import os

class ArgusConan(ConanFile):
    name = "argus"
    version = "0.1.0"
    description = "Modern C library for command-line argument parsing with an elegant, macro-based API"
    topics = ("conan", "argus", "libargus", "command-line", "arguments", "parser", "cli", "argparse")
    url = "https://github.com/lucocozz/argus"
    homepage = "https://github.com/lucocozz/argus"
    license = "MIT"
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "regex": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "regex": True,
    }

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")
        self.settings.rm_safe("compiler.libcxx")
        self.settings.rm_safe("compiler.cppstd")

    def layout(self):
        basic_layout(self, src_folder="src")

    def requirements(self):
        if self.options.regex:
            self.requires("pcre2/[>=10.40]")

    def build_requirements(self):
        self.tool_requires("meson/[>=1.0.0]")
        self.tool_requires("ninja/[>=1.10.2 <2]")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

    def generate(self):
        tc = MesonToolchain(self)
        tc.project_options["buildtype"] = str(self.settings.build_type).lower()
        tc.project_options["regex"] = self.options.regex
        tc.project_options["tests"] = False
        tc.project_options["examples"] = False
        tc.project_options["benchmarks"] = False
        if self.settings.build_type == "RelWithDebInfo":
            tc.project_options["buildtype"] = "release"
        tc.generate()

    def build(self):
        meson = Meson(self)
        meson.configure()
        meson.build()

    def package(self):
        copy(self, "LICENSE", self.source_folder, os.path.join(self.package_folder, "licenses"))
        meson = Meson(self)
        meson.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "argus")
        self.cpp_info.set_property("cmake_target_name", "argus::argus")
        self.cpp_info.libs = ["argus"]
        if self.options.regex:
            self.cpp_info.defines.append("ARGUS_REGEX")
        if self.settings.os == "Linux":
            self.cpp_info.system_libs.append("m")
        self.cpp_info.set_property("cmake_find_mode", "both")
        self.cpp_info.set_property("pkg_config_name", "argus")
