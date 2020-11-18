flags = -pedantic -Werror -Wall --std=c++11 -g

object = PairingPQ.h

a.exe: testPQ.cpp $(object)
	g++ $(flags) testPQ.cpp -o $@

test.exe: test.cpp $(object)
	g++ $(flags) test.cpp -o $@
	./$@

clean:
	rm -rf *.exe