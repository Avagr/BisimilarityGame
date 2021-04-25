#!/bin/bash 

source venv/bin/activate
cd lib
pip install . --use-feature=in-tree-build
deactivate
