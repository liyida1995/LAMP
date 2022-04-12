#############################   Baseline   ####################################
cd ./scripts
rm ./baseline_zfp_ratio.txt

cd ../src
sed -i "11s/.*/#define Level 9/" ./compression_LAMP.c
sed -i "1s/.*/#define Level 9/" ./util.h
make -f Makefile

cd ../Data/Min
../../src/compression_LAMP>>../../scripts/baseline_zfp_ratio.txt
cd ../../
###########################################################################

cd ./src
sed -i "11s/.*/#define Level 5/" ./compression_LAMP.c
sed -i "1s/.*/#define Level 5/" ./util.h
make -f Makefile

cd ../Data/Pin
../../src/compression_LAMP>>../../scripts/baseline_zfp_ratio.txt
cd ../../
###########################################################################

cd ./src
sed -i "11s/.*/#define Level 6/" ./compression_LAMP.c
sed -i "1s/.*/#define Level 6/" ./util.h
make -f Makefile

cd ../Data/Green
../../src/compression_LAMP>>../../scripts/baseline_zfp_ratio.txt
