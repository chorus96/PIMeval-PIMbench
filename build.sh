#1/bin/bash

make -C libpimeval -j8 MAKECMDGOALS=debug
make -C PIMbench/gemv/PIM MAKECMDGOALS=debug
