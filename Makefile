testClient: testClient.cpp
	g++ testClient.cpp -o testClient
testServer: testServer.cpp
	g++ testServer.cpp -o testServer
Cache: Cache.cpp Time.cpp RequestHeader.cpp
	g++ Cache.cpp Time.cpp RequestHeader.cpp -o cache
clean: 
	$(RM) testClient testServer