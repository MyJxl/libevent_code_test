CC = g++
target = test_buffer.out

.PHONY:clean

$(target):test_buffer.cpp
	g++ $^ -o $(target) -levent

clean:
	rm *.out *.o