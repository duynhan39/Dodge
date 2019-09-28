game: nhan.o functions.o
	clang++ -Wall -lcurses -lgdbm -o game nhan.o functions.o

# Compile with -g to retain debugging information
nhan.o: nhan.cpp functions.h
	clang++ -std=c++11 -g -c nhan.cpp

functions.o: functions.cpp functions.h
	clang++ -std=c++11 -g -c functions.cpp
clean:
	rm -f *.o game *~
