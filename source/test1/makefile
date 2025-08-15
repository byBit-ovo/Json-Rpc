INCLUDE = -I../../muduo/release-install-cpp11/include/
LIB     = -L../../muduo/release-install-cpp11/lib/ -lmuduo_net  -lmuduo_base -lpthread -ljsoncpp

.PHONY: clean all
all: server client
server: test_server.cpp
	g++ $^ -o $@ $(INCLUDE) $(LIB) -g
client: test_client.cpp 
	g++ $^ -o $@ $(INCLUDE) $(LIB) -g
clean:
	rm server client
