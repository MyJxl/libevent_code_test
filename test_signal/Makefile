CC = g++
target = test_signal.out

.PHONY:clean

$(target):test_signal.cpp
	g++ $^ -o $(target) -levent

clean:
	rm *.out *.o