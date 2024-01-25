set -xe

valgrind --tool=cachegrind --keep-debuginfo=yes --cache-sim=yes --log-file="cachegrind-%p.txt" /mnt/c/Users/adzic/Desktop/vs/2023_Analysis_24-kalendar/2023_Analysis_24-kalendar/24-kalendar/build-calendar-Desktop-Debug/calendar

latest_file=$(ls -t cachegrind.out.* | head -n1)
cg_annotate "$latest_file" > "$latest_file.txt"