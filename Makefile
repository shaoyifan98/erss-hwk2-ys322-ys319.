testProxy: testProxy.cpp	httpProxy.cpp
	g++ testProxy.cpp httpProxy.cpp Time.cpp RequestHeader.cpp ResponseHeader.cpp -o testProxy -std=gnu++11 -Wall -Werror -pthread
testClient: testClient.cpp ResponseHeader.cpp
	g++ testClient.cpp ResponseHeader.cpp -o testClient
testServer: testServer.cpp RequestHeader.cpp
	g++ testServer.cpp RequestHeader.cpp -o testServer
Cache: Cache.cpp Time.cpp RequestHeader.cpp
	g++ Cache.cpp Time.cpp RequestHeader.cpp -o cache
clean: 
	$(RM) testClient testServer