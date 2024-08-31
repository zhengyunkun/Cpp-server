all: server client th test

src=$(wildcard src/*.cpp)

# -pthread 参数是用来告诉编译器链接pthread库

server:
	g++ -std=c++11 -pthread -g \
	$(src) \
	server.cpp \
	-o server

client:
	g++ src/util.cpp src/Buffer.cpp src/Socket.cpp client.cpp -o client

th:
	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o ThreadPoolTest

test:
	g++ src/util.cpp src/Buffer.cpp src/Socket.cpp src/ThreadPool.cpp \
	-pthread \
	test.cpp -o test

clean:
	rm server && rm client && rm test && rm ThreadPoolTest