INCLUDE = -I../../muduo/release-install-cpp11/include/
LIB     = -L../../muduo/release-install-cpp11/lib/ -lmuduo_net  -lmuduo_base -lpthread -ljsoncpp

.PHONY: clean all
all: server client
server: test_server.cpp
	g++ $^ -o $@ $(INCLUDE) $(LIB) 
client: test_client.cpp 
	g++ $^ -o $@ $(INCLUDE) $(LIB)
clean:
	rm server client
