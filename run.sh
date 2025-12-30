#1/bin/bash

./PIMbench/gemv/PIM/gemv.out -v -t \
                             -c configs/hbm/PIMeval_Aquabolt_Rank8.cfg \
                             2&>1 | tee gemv.out.log

diff gemv.out.orig.log gemv.out.log