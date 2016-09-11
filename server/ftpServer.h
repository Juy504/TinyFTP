#ifndef __FTPSERVER_H__
#define __FTPSERVER_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <sys/wait.h>
#include <vector>
#include <unistd.h>
#include <netdb.h>
#include "../common.h"

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 100
#define _BACKLOG_ 5
#define DIR_SIZE 100
enum Method
{get=0,pwd,ls,cd,quit,put};
extern const char* strMethod[];
class ftpServer
{
public:
	ftpServer();
	~ftpServer();
	int initConnect();
	void processConnect();
private:
	int _port;
	int _connectionCnt;
	int _nMaxUsers;
	int _timeOut;
};
#endif
