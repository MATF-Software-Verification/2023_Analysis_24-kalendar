cd FlameGraph
$sudo perf record -F 99 -ag -- sleep 60
$sudo perf script -i  ./perf.data | ./stackcollapse-perf.pl | ./flamegraph.pl > perf-kernel.svg
$firefox perf-kernel.svg
 sudo perf record -F 99 -ag -- /home/jovana/Desktop/vs/19-under-the-c/UNDER_THE_C/build/Desktop_Qt_5_15_1_GCC_64bit-Debug1/UNDER_THE_C 
 sudo perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > perf-kernel.svg
 
