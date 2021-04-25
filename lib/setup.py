from setuptools import find_packages
from skbuild import setup

setup(
    name="bisimilarity_checker",
    version="1.0",
    author="Agroskin Alexander",
    description="Program for checking resource bisimilarity for Petri nets.",
    url="https://github.com/Avagr/BisimilarityGame",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    include_package_data=True,
    cmake_install_dir="src/bisimilarity_checker",
)
