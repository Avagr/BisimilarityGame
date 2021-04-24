from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

setup(
    name="BisimilarityGame",
    version="1",
    author="Agroskin Alexander",
    description="Program for checking bisimularity for Petri nets.",
    ext_modules=[Pybind11Extension("check_bisimilarity", ["lib/src/binding.cpp"])],
    cmdclass={"build_ext": build_ext},
    url="https://github.com/Avagr/BisimilarityGame"
)
