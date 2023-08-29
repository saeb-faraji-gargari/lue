"""
Conan recipe
"""
import os
import shutil

from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout


def install_conan_package(name):
    result = False

    if "LUE_CONAN_PACKAGES" in os.environ:
        conan_package_names = os.environ["LUE_CONAN_PACKAGES"].split()
        result = name in conan_package_names

    return result


class LUERecipe(ConanFile):
    # Profile-wide variables. Factors that affect binary compatibility.
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):

        if install_conan_package("boost"):
            self.requires("boost/[>1.75]")

        if install_conan_package("docopt.cpp"):
            self.requires("docopt.cpp/[~0.6]")

        if install_conan_package("fmt"):
            self.requires("fmt/[>=9 <10]")

        if install_conan_package("gdal"):
            self.requires("gdal/[~3.4]")

        if install_conan_package("glfw"):
            self.requires("glfw/[~3.3]")

        if install_conan_package("hdf5"):
            self.requires("hdf5/[>=1.12]")

        if install_conan_package("hwloc"):
            self.requires("hwloc/[~2.9]")

        if install_conan_package("imgui"):
            self.requires("imgui/[>=1.89 <2]")

        if install_conan_package("mimalloc"):
            self.requires("mimalloc/[~2.1]")

        if install_conan_package("nlohmann_json"):
            self.requires("nlohmann_json/[~3.10]")

        if install_conan_package("pybind11"):
            self.requires("pybind11/[~2.9]")

        if install_conan_package("span-lite"):
            self.requires("span-lite/[~0.10]")

        if install_conan_package("vulkan-headers"):
            self.requires("vulkan-headers/[~1.3]")

        if install_conan_package("vulkan-loader"):
            self.requires("vulkan-loader/[~1.3]")

        def layout(self):
            cmake_layout(self)

    def generate(self):
        if install_conan_package("imgui"):
            cpp_info = self.dependencies["imgui"].cpp_info
            binding_directory_pathname = cpp_info.srcdirs[0]
            binding_filenames = [
                "imgui_impl_glfw.h",
                "imgui_impl_glfw.cpp",
                "imgui_impl_opengl3.h",
                "imgui_impl_opengl3.cpp",
                "imgui_impl_opengl3_loader.h",
                "imgui_impl_vulkan.h",
                "imgui_impl_vulkan.cpp",
            ]
            output_directory_pathname = os.path.join(
                self.build_folder, "source", "view", "imgui", "src"
            )

            os.makedirs(output_directory_pathname, exist_ok=True)

            for binding_filename in binding_filenames:
                shutil.copy(
                    os.path.join(binding_directory_pathname, binding_filename),
                    output_directory_pathname,
                )

        cmake = CMakeDeps(self)
        cmake.generate()

        toolchain = CMakeToolchain(self, generator="Ninja")
        toolchain.generate()
