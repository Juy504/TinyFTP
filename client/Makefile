ROOT_PATH=$(shell pwd)

#SERVER_BIN=ftp_server
#CLIENT_BIN=ftp_client
#SERVER=$(ROOT_PATH)/server

client:client.o ftpClient.o common.o
	g++ -o $@ $^ -lpthread  
client.o:client.cpp
	g++ -c $< -lpthread -g
ftpClient.o:ftpClient.cpp
	g++ -c $< -lpthread -g
common.o:../common.cpp
	g++ -c $< -lpthread -g
.PHONY:clean
clean:
	rm -f *.o client
