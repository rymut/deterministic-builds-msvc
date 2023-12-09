from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.files import load, copy
from conan.tools.scm import Git
import re
import os

class DeterministicBuildsMsvcConan(ConanFile):
    name = "DeterministicBuildsMsvc"
    description = "Utility for creating deterministic builds under Microsoft Visual C++"
    license = "MIT"
    author = "Boguslaw Rymut (boguslaw@rymut.org)"
    topics = ("microsoft", "msvc", "deterministic-build")
    generators = "CMakeDeps"

    settings = "os", "arch", "compiler", "build_type"
    options = {
    }
    default_options = {
    }

    def set_version(self):
        git = Git(self, self.recipe_folder)
        try:
            if git.is_dirty():
                self.version = "cci_%s" % datetime.datetime.utcnow().strftime('%Y%m%dT%H%M%S')
        except:
            pass
        try:
            tag = git.run("describe --tags").strip()
            if tag.startswith("v"):
                tag = tag[1:].strip()
            if re.match("^(?P<major>0|[1-9]\d*)\.(?P<minor>0|[1-9]\d*)\.(?P<patch>0|[1-9]\d*)(?:-(?P<prerelease>(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+(?P<buildmetadata>[0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$", tag):
                self.version = tag
                return
        except:
            pass
        try:
            self.version = "rev_%s" % git.get_commit().strip()
            return
        except:
            pass
        return None

    def requirements(self):
        self.requires("detours/4.0.1")

    def configure(self):
        pass

    def layout(self):
        self.folders.root = ".."
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

