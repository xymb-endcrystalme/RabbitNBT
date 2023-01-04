import sys

# Available at setup time due to pyproject.toml
from pybind11 import get_cmake_dir
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "1.0.0"

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

ext_modules = [
    Pybind11Extension("rabbitnbt",
        ["src/main.cpp", "src/nbt_util.c", "src/nbt_treeops.c", "src/nbt_parsing.c", "src/nbt_loading.c", "src/buffer.c"],
        # Example: passing in the version to the compiled code
        define_macros = [('VERSION_INFO', __version__)],
        extra_compile_args = ["-O3", "-fpermissive"]
        ),
]

setup(
    name="rabbitnbt",
    version=__version__,
    author="Xymb",
    author_email="xymb@endcrystal.me",
    url="https://github.com/xymb-endcrystalme/RabbitNBT",
    description="Fast NBT library for Python",
    long_description="",
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)
