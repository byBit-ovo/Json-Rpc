INCLUDE = -I../muduo/release-install-cpp11/include/
LIB     = -L../muduo/release-install-cpp11/lib/ -lmuduo_net  -lmuduo_base -lpthread -ljsoncpp

.PHONY: clean
test: test.cpp
	g++ $^ -o $@ $(INCLUDE) $(LIB)
clean:
	rm test