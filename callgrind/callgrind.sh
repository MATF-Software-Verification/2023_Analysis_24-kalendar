set xe

valgrind --tool=callgrind  --keep-debuginfo=yes --callgrind-out-file="callgrind-%p.out" --log-file="callgrind-%p.txt" --dump-instr=yes --collect-jumps=yes --simulate-cache=yes ../19-under-the-c/UNDER_THE_C/build/Desktop_Qt_5_15_1_GCC_64bit-Profile/UNDER_THE_C

echo "finished callgrind"
