from conans import ConanFile, CMake
import os

channel = os.getenv("CONAN_CHANNEL", "testing")
username = os.getenv("CONAN_USERNAME", "hoxnox")

class SnappyTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "yadisk-upload/0.0.6@%s/%s" % (username, channel)
    default_options = "yadisk-upload:tools=False"
    generators = "cmake"

    def configure(self):
        self.options["yadisk-upload"].log = self.conanfile_directory
        self.options["yadisk-upload"].libressl = False

    def build(self):
        cmake = CMake(self.settings)
        self.run('cmake "%s" %s' % (self.conanfile_directory, cmake.command_line))
        self.run("cmake --build . %s" % cmake.build_config)

    def imports(self):
        self.copy(pattern="*.dll"   , dst="bin", src="bin")
        self.copy(pattern="*.dylib*", dst="bin", src="lib")
        self.copy(pattern="*.so*"   , dst="lib", src="lib")

    def test(self):
        os.chdir("bin")
        self.run(".%stest" % os.sep)
