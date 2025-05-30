#Author: realyoavperetz@gmail.com

.PHONY: Main test valgrind clean

Main:
	g++ -std=c++17 -Wall -Wextra -pedantic Demo.cpp -Iinclude -o Main
	./Main

test:
	g++ -std=c++17 -Wall -Wextra -pedantic tests.cpp -Iinclude -o tests
	./tests

valgrind: Main test
	./tests
	valgrind --leak-check=full --show-leak-kinds=all ./tests
	./Main
	valgrind --leak-check=full --show-leak-kinds=all ./Main

clean:
	rm -f Main tests