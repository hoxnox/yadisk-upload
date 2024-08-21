from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake
from conan.tools.files import get,copy

class YadiskUploadConan(ConanFile):
    name = "yadisk_upload"
    description = "Yandex disk file upload library and tool"
    url = "https://github.com/hoxnox/yadisk-upload"
    license = "http://github.com/hoxnox/yadisk-upload/blob/master/LICENSE"
    settings = "os", "compiler", "build_type", "arch"
    options = {"tools":[True, False], "libressl":[True, False], "log":["ANY"]}
    default_options = {"tools": True, "libressl": False, "log": ""}
    requires = "boost/[>=1.44.0]"
    test_requires = "gtest/[>=1.8.0]"

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)
        #copy(self, "*", src="/home/h/devel/yadisk-upload", dst=self.source_folder)


    def requirements(self):
        if self.options.libressl:
            self.requires("libressl/[>=2.9.2 < 3.0.0]")
        else:
            self.requires("openssl/[>=1.1.0 <1.2.0]")
        if self.options.tools:
            self.requires("docopt.cpp/[>=0.6.3]")
        if len(str(self.options.log)) == 0 or self.options.tools:
            self.requires("easyloggingpp/[>=9.96.7]")

    def generate(self):
       deps = CMakeDeps(self)
       deps.generate()
       tc = CMakeToolchain(self)
       tc.variables["WITH_LIBS"] = True
       tc.variables["WITH_CONAN"] = True
       tc.variables["WITH_TOOLS"] = self.options.tools
       if len(str(self.options.log)) != 0:
           tc.variables["WITH_LOG"] = self.options.log
       tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
    
    def package_info(self):
        self.cpp_info.libs = ["yandex_api"]
