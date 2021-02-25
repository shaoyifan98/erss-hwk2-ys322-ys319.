testClient: testClient.cpp
	g++ testClient.cpp -o testClient
testServer: testServer.cpp
	g++ testServer.cpp -o testServer
clean: 
	$(RM) testClient testServer