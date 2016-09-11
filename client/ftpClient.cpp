#include "ftpClient.h"

int sockfds[socknum];
void print_log(const char* fun,int line,int err_no,const char* err_str)
{
	cout<<fun<<" : "<<line<<err_no<<err_str<<endl;
}
ftpClient::ftpClient()
	:host("127.0.0.1"),_port(21)
{}
ftpClient::~ftpClient()
{}


int pasv(char *port)
{
	struct sockaddr_in client;
	int fd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&client,sizeof(client));
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = inet_addr("127.0.0.1");
	client.sin_port = htons(atoi(port));
	if(connect(fd,(struct sockaddr *)&client,sizeof(client)) < 0)
	{
		cout<<"connect failure"<<endl;
	}
	else
	{
		cout<<"connect success"<<endl;
	}
	return fd;
}

void* ftp_get(void* arg)
{
	cout<<"enter ftp_get"<<endl;
	char* port = (char*)arg;
	int sockfd = pasv(port);
//	struct sockaddr_in client;
//	int sockfd = socket(AF_INET,SOCK_STREAM,0);
//	bzero(&client,sizeof(client));
//	client.sin_family = AF_INET;
//	client.sin_addr.s_addr = inet_addr("127.0.0.1");
//	client.sin_port = htons(atoi(port));
//	if(connect(sockfd,(struct sockaddr *)&client,sizeof(client)) < 0)
//	{
//		cout<<"connect failure"<<endl;
//	}
//	else
//	{
//		cout<<"connect success"<<endl;
		const char* path = "1.txt";
		char pathRemote[100];
		memset(pathRemote,'\0',sizeof(pathRemote)-1);
		cout<<"Please input remote filename > ";
		cin>>pathRemote;
		int RemoteLen = strlen(pathRemote);
		cout<<"RemoteLen : > "<<RemoteLen<<endl;
		if((send(sockfd,pathRemote,RemoteLen,0)) < 0)
		{
			cout<<"send remote path failure"<<endl;
		}
		else
			cout<<"send success !"<<endl;
	
		FILE* FD = fopen(path,"w+");
		char recvBuf[256];
		int recvLen = 0;
	
		if(FD == NULL )
		{
			cout<<"fopen failure!"<<endl;
			return NULL;
		}
		while((recvLen = recv(sockfd,recvBuf,sizeof(recvBuf),0)) >0 )
		{
			int num = fwrite(recvBuf,recvLen,1,FD);
			if(num < 0)
			{
				cout<<"fwrite failure !"<<endl;
				return NULL;
			}
			else
			{
				cout<<"fwrite success ! "<<endl;
			}
		}
		fclose(FD);
//	}
	close(sockfd);
}

void* ftp_put(void* arg)
{
	cout<<"enter ftp_put"<<endl;
	char* port = (char*)arg;
	int sockfd = pasv(port);

	if(sockfd > 0)
	{
		cout<<"connect success!"<<endl;
	}else
		cout<<"connect failure!"<<endl;

	const char* path = "/home/jy/tinyftp/client/1.txt";
	char pathRemote[100];
	char buf[65535];
	memset(buf,'\0',sizeof(buf));
	memset(pathRemote,'\0',sizeof(pathRemote));
	cout<<"Please input remote filename > ";
	cin>>pathRemote;
	int RemoteLen = strlen(pathRemote);
		
	if((send(sockfd,pathRemote,strlen(pathRemote),0)) < 0)
	{
		cout<<"send failure!"<<endl;
	}else
		cout<<"send success"<<endl;
	sleep(2);	
	FILE* FD = fopen(path,"r");
	
	if(FD == NULL)
	{
		cout<<"fopen failure"<<endl;
		return NULL;
	}
	else
	{
		cout<<"fopen success"<<endl;
		int num = fread(buf,sizeof(char),sizeof(buf),FD);
		cout<<"num :> "<<num<<endl;
		send(sockfd,buf,num,0);
		cout<<"sending......"<<endl;
		fclose(FD);
	}
	close(sockfd);
}

char* ftp_pwd(char* buf,int sockfd)
{
	assert(buf !=NULL);
	memset(buf,'\0',sizeof(buf));
	int i=0;
	if(recv(sockfd,buf,sizeof(buf),0) > 0)
	{
		cout<<"buf : >"<<buf<<endl;
		char* p = buf+3;
		while(*p)
			buf[i++] = *p++;
		buf[i] = '\0';
		cout<<"recv success"<<endl;
		cout<<"curr_dir :>"<<buf<<endl;
	}
	return buf;
}

void* ftp_list(void* arg)
{
	cout<<"enter ftp_list"<<endl;
	char* port = (char*)arg;
	int sockfd = pasv(port);
	char buf[100];

	if(sockfd < 0)
	{
		cout<<"connect failure!"<<endl;
	}else
	{
		cout<<"connect success!"<<endl;
		memset(buf,'\0',sizeof(buf)-1);
		if(recv(sockfd,buf,sizeof(buf),0) < 0)
		{
			cout<<"recv failure"<<endl;
		}else
			cout<<"remote dir :>"<<buf<<endl;
	}
	
	struct dirent *ptr = NULL;
	DIR* dir = opendir(buf);
	char buff[1024];
	memset(buff,'\0',sizeof(buff));
	while((ptr = readdir(dir)) != NULL)
	{
		if(ptr->d_name[0] == '.')
			continue;
		cout<<ptr->d_name<<endl;
		sprintf(buff," %s",ptr->d_name);
	}
	cout<<"server/ "<<buff<<endl;
	cout<<endl;
	close(sockfd);
}

void* ftp_cd(void* arg)
{
	cout<<"enter ftp_cd"<<endl;
	char* port = (char*)arg;
	int sockfd = pasv(port);
	char buf[100];

	if(sockfd < 0)
	{
		cout<<"connect failure!"<<endl;
	}else
	{
		cout<<"connect success!"<<endl;
		memset(buf,'\0',sizeof(buf)-1);

		char remoteDir[20];
		memset(remoteDir,'\0',sizeof(remoteDir));
		cout<<"Please input remote dir :>";
		cin>>remoteDir;
		send(sockfd,remoteDir,strlen(remoteDir),0);
	}
	close(sockfd);
}

void process_cmd(int sockfd,char* host)
{
	char path[100];
	char cmd[10];
	char buf[100];
	char port[5];
	memset(port,'\0',sizeof(port)-1);
	memset(path,'\0',sizeof(path)-1);
	memset(cmd,'\0',sizeof(cmd)-1);
	memset(buf,'\0',sizeof(buf)-1);
	if(recv(sockfd,port,4,0) >0)
	{
		cout<<"recv success"<<endl;
		port[4] ='\0';
	}
	cout<<"PORT : > "<<port<<endl;
	while(1)
	{

		sleep(1);
		cout<<"ftp"<<">";
		cin>>cmd;
		int cmdLen = strlen(cmd);
		send(sockfd,cmd,cmdLen,0);
		sleep(1);
		recv(sockfd,buf,sizeof(buf),0);
		
		if(strncmp(buf,"257",3) == 0)
		{
			ftp_pwd(buf,sockfd);
			memset(buf,'\0',sizeof(buf)-1);
			continue;
		}
		if(strncmp(buf,"250",3) == 0)
		{
			pthread_t cd;
			pthread_create(&cd,NULL,ftp_cd,port);
			pthread_join(cd,NULL);
		}
		if(strncmp(buf,"227",3) == 0)
		{
			pthread_t list;
			pthread_create(&list,NULL,ftp_list,port);
			pthread_join(list,NULL);
		}
		if(strncmp(buf,"220",3) == 0)
		{
	//		cout<<"sockfd : >"<<sockfd<<endl;
	//		int fd = pasv(port,&sockfd);
	//		cout<<"pasv fd"<<fd<<endl;
			pthread_t get;
			pthread_create(&get,NULL,ftp_get,port);
			
			pthread_join(get,NULL);
		}
		if(strncmp(buf,"200",3) == 0)
		{
			pthread_t put;
			pthread_create(&put,NULL,ftp_put,port);
			pthread_join(put,NULL);
		}
		if(strncmp(buf,"quit",4) == 0)
		{
			cout<<"bye bye ^=^!"<<endl;
			exit(0);
		}
		else
		{
			continue;
		}
	}
}   	
    	
static void Login(int sockfd,char* host)
{   	
	char name[20];
	char passwd[20];
	char PORT[5];
	memset(name,'\0',sizeof(name)-1);
	memset(passwd,'\0',sizeof(passwd)-1);
	memset(PORT,'\0',sizeof(PORT)-1);
	char buf[BUFF_SIZE];
	memset(buf,'\0',BUFF_SIZE-1);
	
	const char* cmd = "hello server";
	int len = strlen(cmd);
	ssize_t size = send(sockfd,cmd,len,0);//send hello server
	
	recv(sockfd,buf,sizeof(buf),0);//recv 220
	cout<<buf<<endl;
	
	cout<<"Name "<<'('<<host<<":jy):"<<" ";
	cin>>name;
	int lenName = strlen(name);
	send(sockfd,name,lenName,0);//send name

	memset(buf,'\0',BUFF_SIZE-1);
	recv(sockfd,buf,sizeof(buf),0);//recv 331 specify passwd
	cout<<buf<<endl;

	cout<<"Password :"<<" ";
	cin>>passwd;
	int lenPasswd = strlen(passwd);
	send(sockfd,passwd,lenPasswd,0);

	sleep(1);
	memset(buf,'\0',BUFF_SIZE-1);
	recv(sockfd,buf,sizeof(buf),0);//recv remote message
	cout<<buf<<endl;

	cout<<"pthread_create before > "<<sockfd<<endl;

//	sleep(1);	
//	cout<<"before recv > "<<PORT<<endl;
//	if(recv(sockfd,PORT,4,0) >0)
//	{
//		cout<<"recv success"<<endl;
//		PORT[4] ='\0';
//	}
//	cout<<"PORT : > "<<PORT<<endl;

	process_cmd(sockfd,host);	
	close(sockfd);
}

void ftpClient::init()
{
	struct sockaddr_in client;
	for(int i=0;i<socknum; i++)
	{
		sockfds[i] = socket(AF_INET,SOCK_STREAM,0);
		bzero(&client,sizeof(client));
		client.sin_family = AF_INET;
		client.sin_addr.s_addr = inet_addr(host);
		client.sin_port = htons(_port);
		connect(sockfds[i],(struct sockaddr *)&client,sizeof(client));

		
		Login(sockfds[i],host);
	}
}
