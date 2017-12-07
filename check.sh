g++ -std=c++11 -o a pathselection.cpp
g++ -std=c++11 -o b bruteforce.cpp
g++ -std=c++11 -o redblack rbtree.cpp
/usr/bin/time -v ./a wt.csv < input.txt > outa.txt
/usr/bin/time -v ./b bf.csv < input.txt > outb.txt
diff outa.txt outb.txt
echo "A and B"
/usr/bin/time -v ./redblack rb.csv < input.txt > outc.txt
diff outa.txt outc.txt
echo "A and C"
# diff outa.txt outb.txt
