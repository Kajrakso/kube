from Cython.Build import cythonize
from setuptools import Extension, setup

extensions = [
    Extension("libcube",
        sources=["libcube.pyx"],
        include_dirs = ["../include"],
        extra_objects=["../libcube.a"],
    )
]

setup(
    ext_modules=cythonize(
        extensions,
        annotate=True,
    )
)