INCLUDE = -I../../muduo/release-install-cpp11/include/
LIB     = -L../../muduo/release-install-cpp11/lib/ -lmuduo_net  -lmuduo_base -lpthread -ljsoncpp

.PHONY: clean all
all: server publish subscribe
server: server.cpp
	g++ $^ -o $@ $(INCLUDE) $(LIB) -g
publish: publish_client.cpp 
	g++ $^ -o $@ $(INCLUDE) $(LIB) -g
subscribe: subscribe_client.cpp 
	g++ $^ -o $@ $(INCLUDE) $(LIB) -g
clean:
	rm server publish subscribe
