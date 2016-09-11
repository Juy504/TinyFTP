#ifndef __FTPCLIENT_H__
#define __FTPCLIENT_H__

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <string>
#include <netdb.h>
#include "../common.h"
#include <errno.h>
#include <assert.h>
#include <dirent.h>

using namespace std;
#define socknum  5
#define BUFF_SIZE 1024
class ftpClient
{
public:
	ftpClient();
	virtual~ftpClient();
	void init();
private:
	char* host;
	unsigned short _port;
};
#endif
