#include "ftpServer.h"

int main(int argc,char* argv[])
{
	ftpServer ftp;
//	int listenfd = ftp.initConnect();
	ftp.processConnect();
	return 0;
}
