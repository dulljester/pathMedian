java -ea RandBracketSequence 100000 7 > out.txt
java -ea BracketsToTree < out.txt > input.txt
./diam < input.txt
