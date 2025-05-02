from Cython.Build import cythonize
from setuptools import Extension, setup

setup(
    ext_modules = cythonize([
        Extension("libcube",
            sources=["libcube.pyx"],
            include_dirs = ["../include"],
            extra_objects=["../libcube.a"],
        )
    ])
)