INCLUDE = -I../muduo/release-install-cpp11/include/
LIB     = -L../muduo/release-install-cpp11/lib/ -lmuduo_net  -lmuduo_base -lpthread -ljsoncpp

.PHONY: clean all
all: server client
server: server.cpp
	g++ $^ -o $@ $(INCLUDE) $(LIB) 
client: client.cpp 
	g++ $^ -o $@ $(INCLUDE) $(LIB)
clean:
	rm server client
