FLAGS = --std=c++11 -Wall -Werror -pedantic -g

drone: main.cpp drone.o
	g++ $(FLAGS) $^ -o $@

drone.o: Drone.cpp
	g++ $(FLAGS) -c $^ -o $@

clean:
	rm -rf *.o drone