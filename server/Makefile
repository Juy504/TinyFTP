ROOT_PATH=$(shell pwd)

#SERVER_BIN=ftp_server
#CLIENT_BIN=ftp_client
#SERVER=$(ROOT_PATH)/server

server:server.o ftpServer.o common.o
	g++ -o $@ $^ -lpthread  
server.o:server.cpp
	g++ -c $< -lpthread -g
ftpServer.o:ftpServer.cpp
	g++ -c $< -lpthread -g
common.o:../common.cpp
	g++ -c $< -lpthread -g
.PHONY:clean
clean:
	rm -f *.o server
