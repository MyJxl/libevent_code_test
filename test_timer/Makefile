CC = g++
target = test_timer.out

.PHONY:clean

$(target):test_timer.cpp
	g++ $^ -o $(target) -levent

clean:
	rm *.out *.o