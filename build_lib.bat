@echo off

call venv\Scripts\activate.bat

cd lib

pip install . --use-feature=in-tree-build

deactivate
