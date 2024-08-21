set -xe

sudo perf record  --call-graph dwarf /home/jovana/Desktop/vs/19-under-the-c/UNDER_THE_C/build/Desktop_Qt_5_15_1_GCC_64bit-Debug1/UNDER_THE_C

echo "Generise se izvestaj ..."
sudo perf report 
