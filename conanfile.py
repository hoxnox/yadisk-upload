from conans import ConanFile


class YadiskUploadConan(ConanFile):
    name = "yadisk-upload"
    description = "Yandex disk file upload library and tool"
    version = "0.0.5"
    url = "https://github.com/hoxnox/yadisk-upload"
    license = "http://github.com/hoxnox/yadisk-upload/blob/master/LICENSE"
    settings = "os", "compiler", "build_type", "arch"
    options = {"tools":[True, False]}
    default_options = "tools=True"
    build_policy = "missing"
    requires = (("boost/1.64.0@hoxnox/testing"),
                ("libressl/2.5.3@hoxnox/testing"),
                ("docopt.cpp/0.6.2@hoxnox/testing"),
                ("easyloggingpp/9.89@hoxnox/testing"),
                ("gtest/1.8.0@hoxnox/testing"))
    generators = "cmake"


