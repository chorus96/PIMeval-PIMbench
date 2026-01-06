#1/bin/bash

if [ ! -f libpimeval/lib/libpimeval.a ]; then
    make -C libpimeval -j8 MAKECMDGOALS=debug
fi

if [ ! -f ./PIMbench/gemv/PIM/gemv.out ]; then
    make -C PIMbench/gemv/PIM MAKECMDGOALS=debug
fi

./PIMbench/gemv/PIM/gemv.out -v -t \
                             -c configs/hbm/PIMeval_Aquabolt_Rank8.cfg \
                             2>&1 | tee gemv.out.log

diff gemv.out.orig.log gemv.out.log