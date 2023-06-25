import pip
import os
import setuptools


try:
    from pybind11.setup_helpers import Pybind11Extension
except Exception as e:
    print(e)
    pip.main(["install", "pybind11"])

import setuptools
from pybind11.setup_helpers import Pybind11Extension



def build():
    vcpkg_path = "E:\\vcpkg"
    workspace = os.path.dirname(__file__)
    include_dir = os.path.join(os.path.dirname(workspace), "FastFileSearch\\")

    extension = Pybind11Extension("FFS",
                                  sources=[os.path.join(workspace, "src\\main.cpp")],
                                  libraries=["brotlienc","brotlidec", "brotlicommon"],
                                  include_dirs=[include_dir,  f"{vcpkg_path}\\installed\\x64-windows\\include"],
                                  library_dirs=[f"{vcpkg_path}\\installed\\x64-windows\\lib"],
                                  extra_compile_args=['/std:c++20'],
                                  extra_link_args=['/MACHINE:X64'])

    setuptools.setup(
        name="FFS",
        version="0.0.1",
        author="Jules Garcia",
        description="",
        ext_modules=[extension])


if __name__ == "__main__":
    build()
