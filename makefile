clean:
	rm -f a.out

run:
	g++ *.cpp -std=c++2a -o a.out
	./a.out ../lab1samples/input-1