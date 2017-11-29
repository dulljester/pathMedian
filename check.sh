g++ -std=c++11 -o a pathselection.cpp
g++ -std=c++11 -o b bruteforce.cpp
./a < input.txt > outa.txt
./b < input.txt > outb.txt
# diff outa.txt outb.txt
