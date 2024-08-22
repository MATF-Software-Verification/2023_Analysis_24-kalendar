set -xe

valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file="$(date +%s).memcheck.out" ../19-under-the-c/UNDER_THE_C/build/Desktop_Qt_5_15_1_GCC_64bit-Debug/UNDER_THE_C

echo "finished memcheck"
