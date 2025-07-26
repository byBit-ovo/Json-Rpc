INCLUDE = -I ../muduo/release-install-cpp11/include/
LIB     = -L  ../muduo/release-install-cpp11/lib/ -lmuduo_net  -lmuduo_base -lpthread

.PHONY:all clean
all: DictServer DictClient

DictServer: testMuduoDict.cpp
	g++ $^ -o $@ -std=c++11 $(INCLUDE) $(LIB) 
DictClient:client.cpp
	g++ $^ -o $@ -std=c++11 $(INCLUDE) $(LIB) 

clean:
	rm DictServer