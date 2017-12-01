g++ -std=c++11 -o a pathselection.cpp
g++ -std=c++11 -o b bruteforce.cpp
g++ -std=c++11 -o redblack rbtree.cpp
./a stat_wvt.txt < input.txt > outa.txt
./b stat_bf.txt < input.txt > outb.txt
diff outa.txt outb.txt
echo "A and B"
./redblack stat_rb.txt < input.txt > outc.txt
diff outa.txt outc.txt
echo "A and C"
# diff outa.txt outb.txt
