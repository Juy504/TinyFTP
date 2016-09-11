#include "common.h"

char* itoa(int integer,char* str)
{
	int pow,j;
	j = integer;
	int i = 0;
	for(pow = 1;j>=10;j/=10)
		pow *= 10;
	for(;pow>0;pow/=10)
	{
		str[i++] = '0' + integer/pow;
		integer%=pow;
	}
	str[i] = '\0';
	return str;
}

char* beforeConn(char* pIP)
{
	cout<<"enter beforeConn"<<endl;
	char hostName[50];
	in_addr addr;
	gethostname(hostName,50);

	struct hostent *pHost;
	pHost = gethostbyname(hostName);
	memcpy(&addr.s_addr,pHost->h_addr_list[0],pHost->h_length);
	pIP = inet_ntoa(addr);

	return pIP;
}
