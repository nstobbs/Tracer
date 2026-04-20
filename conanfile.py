from conan import ConanFile
from conan.tools.cmake import cmake_layout

class TracerConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        # Workaround for building Debug on Linux
        if self.settings.os == "Linux":
            self.requires("m4/1.4.20", override=True)
            
        self.requires("glew/2.2.0")
        self.requires("glm/1.0.1")
        self.requires("assimp/5.4.3")
        self.requires("sdl/2.28.3")
        self.requires("sdl_image/2.6.3")

    def layout(self):
        cmake_layout(self)