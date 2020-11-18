flags = -pedantic -Werror -Wall --std=c++11 -g

a.exe: main.cpp getmode.o Hunt.o
	g++ $(flags) $^ -o $@

getmode.o: getMode.cpp
	g++ $(flags) -c $^ -o $@

Hunt.o: Hunt.cpp
	g++ $(flags) -c $^ -o $@

clean:
	rm -rf *.o *.exe