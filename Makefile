server:
	g++ src/util.cpp client.cpp -o client && \
	g++ src/util.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp src/Channel.cpp \
	 	src/EventLoop.cpp src/Server.cpp src/Acceptor.cpp src/Connection.cpp src/Buffer.cpp \
		server.cpp src/ThreadPool.cpp -o server
clean:
	rm server && rm client