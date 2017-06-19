from nxtools import NxConanFile
from conans import CMake, tools
from shutil import copy


class YadiskUploadConan(NxConanFile):
    name = "yadisk-upload"
    description = "Yandex disk file upload library and tool"
    version = "0.0.7"
    url = "https://github.com/hoxnox/yadisk-upload"
    license = "http://github.com/hoxnox/yadisk-upload/blob/master/LICENSE"
    settings = "os", "compiler", "build_type", "arch"
    options = {"tools":[True, False], "libressl":[True, False], "tests":[True, False], "log":"ANY"}
    default_options = "tools=True", "libressl=True", "tests=False", "log="
    build_policy = "missing"
    requires = ("boost/1.64.0@hoxnox/stable")
    generators = "cmake"

    def requirements(self):
        if self.options.libressl:
            self.requires("libressl/2.5.3@hoxnox/stable")
        else:
            self.requires("openssl/1.1.0e@hoxnox/stable")

        if self.options.tests:
            self.requires("gtest/1.8.0@hoxnox/stable")

        if self.options.tools:
            self.requires("docopt.cpp/0.6.2@hoxnox/stable")

        if len(str(self.options.log)) == 0 or self.options.tools:
            self.requires("easyloggingpp/9.89@hoxnox/stable")

    def do_source(self):
        self.retrieve("b688cf69e69dd598f523031d52576c6af9172caa0d8fab276f34d92527435743",
            [
                'vendor://hoxnox/yadisk-upload/yadisk-upload-{version}.tar.gz'.format(version=self.version),
                'https://github.com/hoxnox/yadisk-upload/archive/{version}.tar.gz'.format(version=self.version)
            ], "yadisk-upload-{v}.tar.gz".format(v = self.version))

    def do_build(self):
        cmake = CMake(self)
        cmake.build_dir = "{staging_dir}/src-yadisk-upload".format(staging_dir=self.staging_dir)
        tools.untargz("yadisk-upload-{v}.tar.gz".format(v=self.version), cmake.build_dir)
        copy('conanbuildinfo.cmake', cmake.build_dir)
        cmake_defs = {"CMAKE_INSTALL_PREFIX": self.staging_dir,
                      "CMAKE_INSTALL_LIBDIR": "lib",
                      "WITH_TESTS": "1" if self.options.tests else "0",
                      "WITH_LIBS": "1",
                      "WITH_TOOLS": "1" if self.options.tools else "0"}
        if len(str(self.options.log)) != 0:
            cmake_defs.update({"WITH_LOG": self.options.log})
        cmake_defs.update(self.cmake_crt_linking_flags())
        cmake_defs.update(self.cmake_crt_linking_flags())
        cmake.configure(defs=cmake_defs, source_dir="yadisk-upload-{v}".format(v=self.version))
        cmake.build(target="install")

    def do_package_info(self):
        self.cpp_info.libs = ["yandex_api"]


