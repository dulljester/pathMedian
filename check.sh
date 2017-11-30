g++ -std=c++11 -o a pathselection.cpp
g++ -std=c++11 -o b bruteforce.cpp
g++ -std=c++11 -o redblack rbtree.cpp
./a < input.txt > outa.txt
./b < input.txt > outb.txt
./redblack < input.txt > outc.txt
diff outa.txt outb.txt
diff outa.txt outc.txt
# diff outa.txt outb.txt
