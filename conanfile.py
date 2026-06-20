import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy

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
        self.requires("imgui/1.91.4")

    def generate(self):
        copy(self, "imgui_impl_sdl2*", 
             os.path.join(self.dependencies["imgui"].package_folder, "res", "bindings"),
             os.path.join(self.source_folder, "Application", "Window", "imgui_bindings"))
        copy(self, "imgui_impl_opengl3*",
             os.path.join(self.dependencies["imgui"].package_folder, "res", "bindings"),
             os.path.join(self.source_folder, "Application", "Window", "imgui_bindings"))
        copy(self, "*.h",
             os.path.join(self.dependencies["imgui"].package_folder, "res", "misc", "cpp"),
             os.path.join(self.source_folder, "Application", "Window", "imgui_misc"))
        copy(self, "*.cpp",
             os.path.join(self.dependencies["imgui"].package_folder, "res", "misc", "cpp"),
             os.path.join(self.source_folder, "Application", "Window", "imgui_misc"))

    def layout(self):
        cmake_layout(self)