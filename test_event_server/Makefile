CC = g++
target = test_event_server.out

.PHONY:clean

$(target):test_event_server.cpp
	g++ $^ -o $(target) -levent

clean:
	rm *.out *.o