flags = -pedantic -Werror -Wall --std=c++11 -g
target = main.cpp

a.exe: $(target)
	g++ $(flags) $^ -o $@
	./$@

clean:
	rm -rf *.exe
