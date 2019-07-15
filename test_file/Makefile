CC = g++
target = test_file.out

.PHONY:clean

$(target):test_file.cpp
	g++ $^ -o $(target) -levent

clean:
	rm *.out *.o