#include "ftpServer.h"
#include <iostream>
using namespace std;


const char* strMethod[] = {"get","pwd","ls","cd","quit","put"};
enum Method i; 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
void print_log(const char* fun,int line,int err_no,const char* err_str)
{
	printf("[%s : %d] [%d][%s]\n",fun,line,err_no,err_str);
}
ftpServer::ftpServer()
{
	_port = 21;
	_connectionCnt = 0;
	_nMaxUsers = 10;
	_timeOut = 5;
}

ftpServer::~ftpServer()
{
//	stop();
}
static int start(int port)
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1)
	{
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(1);
	}
	int flag = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t len = sizeof(local);
	if(bind(sock,(const struct sockaddr*)&local,len) == -1)
	{
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(2);
	}
	if(listen(sock,_BACKLOG_) == -1)
	{
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(3);
	}
	return sock;
}

Method& operator++(Method& M)//prev ++
{
	int i = M;
	M = (enum Method)(i+1);
	return M;
}

void* ftp_put(void* arg)
{
	pthread_detach(pthread_self());
	char* port = (char*)arg;

	struct sockaddr_in local;
	socklen_t len = sizeof(local);
	int sock = start(atoi(port));

	int fdClient = accept(sock,(struct sockaddr*)&local,&len);
	if(fdClient == -1)
	{
		cout<<"accept failure!"<<endl;
	}
	else
	{
		cout<<"accept success!"<<endl;
		char buf[100];
		memset(buf,'\0',sizeof(buf));

		if(recv(fdClient,buf,sizeof(buf),0) < 0)
		{
			cout<<"recv local failure!"<<endl;
		}
		else
		{
			cout<<"recv local buf : >"<<buf<<endl;
			char recvBuf[65535];
			int recvLen = 0;
			memset(recvBuf,'\0',sizeof(recvBuf));
		
			FILE* FD = fopen(buf,"w+");
			if(FD == NULL)
			{
				cout<<"fopen failure"<<endl;
				return NULL;
			}
			else
			{
				cout<<"enter recving"<<endl;
				while((recvLen = recv(fdClient,recvBuf,sizeof(recvBuf),0)) > 0)
				{
					int num = fwrite(recvBuf,recvLen,1,FD);
					if(num < 0)
					{
						cout<<"fwrite failure"<<endl;
						return NULL;
					}
					else
						cout<<"fwrite success"<<endl;
				}
				fclose(FD);
			}

		}
	}
	shutdown(fdClient,2);
}

void* ftp_get(void* arg)
{
	pthread_detach(pthread_self());

	char* port = (char*)arg;

	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	int sock = start(atoi(port));
	int fdClient = accept(sock,(struct sockaddr*)&client,&len);

	if(fdClient == -1)
	{
		cout<<"accept failure"<<endl;
	}
	else
	{
		cout<<"accept success"<<endl;
		char buf[100];
		memset(buf,'\0',sizeof(buf));
		
		if((recv(fdClient,buf,sizeof(buf),0)) < 0)
		{
			cout<<"recv local failure"<<endl;
		}else
		{
			cout<<"recv buf : >"<<buf<<endl;
			FILE* FD = fopen(buf,"r");
			if(FD ==NULL)
			{
				cout<<"fopen failure"<<endl;
				return NULL;
			}
			else
				cout<<"fopen success"<<endl;
			int num = fread(buf,sizeof(char),sizeof(buf),FD);
			
			cout<<"num :> "<<num<<endl;
			send(fdClient,buf,num,0);
			cout<<"sending ......"<<endl;
			fclose(FD);
		}
	}
//	close(fdClient);
	shutdown(fdClient,2);

}

void ftp_pwd(int sockfd)
{
	char curr_dir[DIR_SIZE];
	memset(curr_dir,'\0',DIR_SIZE-1);
	if(getcwd(curr_dir,DIR_SIZE) == NULL)
	{
		cout<<"error"<<endl;
	}
	else
	{
		cout<<"curr_dir : >"<<curr_dir<<endl;
		if((send(sockfd,curr_dir,strlen(curr_dir),0))<0)
			cout<<"send failure"<<endl;
		else
			cout<<"send success"<<endl;
	}
	shutdown(sockfd,2);
}

void* ftp_list(void* arg)
{
	pthread_detach(pthread_self());
	char* port = (char*)arg;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	int sock = start(atoi(port));
	int fdClient = accept(sock,(struct sockaddr*)&client,&len);

	if(fdClient == -1)
	{
		cout<<"accept failure"<<endl;
	}
	else
	{
		cout<<"accept success"<<endl;
		
		char curr_dir[DIR_SIZE];
		memset(curr_dir,'\0',DIR_SIZE-1);
		if(getcwd(curr_dir,DIR_SIZE) == NULL)
		{
			cout<<"error"<<endl;
		}
		else
		{
			cout<<"getcwd success :>"<<curr_dir<<endl;
			if(send(fdClient,curr_dir,strlen(curr_dir),0)<0)
			{
				cout<<"send curr_dir failure"<<endl;
			}
			else
				cout<<"send success curr_dir : >"<<curr_dir<<endl;
		}
	}
	shutdown(fdClient,2);

}

void* ftp_cd(void* arg)
{
	pthread_detach(pthread_self());
	char* port = (char*)arg;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	int sock = start(atoi(port));
	int fdClient = accept(sock,(struct sockaddr*)&client,&len);

	if(fdClient == -1)
	{
		cout<<"accept failure"<<endl;
	}
	else
	{
		cout<<"accept success"<<endl;
		char goalDir[100];
		char dir[100];
		memset(dir,'\0',sizeof(dir)-1);
		memset(goalDir,'\0',sizeof(goalDir)-1);
		recv(fdClient,dir,sizeof(dir),0);

		if(getcwd(goalDir,sizeof(goalDir)-1) == NULL)
			cout<<"getcwd error"<<endl;
		else
		{
			strcat(goalDir,"/");
			strcat(goalDir,dir);
			cout<<"goalDir :>"<<goalDir<<endl;
			if(chdir(goalDir) == -1)
				cout<<"cd failure"<<endl;
			else
				cout<<"cd success"<<endl;
		}
	}
	//close(fdClient);
}
void process_mnd(int sockfd)
{
	char str[5];
	char method[10];
	const char* method1 = "220 get method";
	const char* method2 = "200 put method";
	int len = 0;
	memset(method,'\0',sizeof(method)-1);
	int randPort = 1027+rand()%100;
	char* port = itoa(randPort,str); //server create a rand port
	if(send(sockfd,port,strlen(port),0) == -1)
	{
		cout<<"send failure"<<endl;
	}
	else
	{
		cout<<port<<endl;
		cout<<"send success"<<endl;
	}
	while(1)
	{
		sleep(1);
		recv(sockfd,method,sizeof(method),0);
		
		for(i=get ;i<=put;++i)
		{
			if(!strncmp(method,strMethod[i],3))
				break;
		}
		
		switch(i)//default pasv
		{
		case ls:
			cout<<"ls method"<<endl;
			send(sockfd,"227",3,0);
			pthread_t ls;
			pthread_create(&ls,NULL,ftp_list,port);
			break;
		case cd:
			cout<<"cd method"<<endl;
			send(sockfd,"250",3,0);
			pthread_t cd;
			pthread_create(&cd,NULL,ftp_cd,port);
			break;
		case get:
			cout<<"get method"<<endl;
			
			len = strlen(method1);
			send(sockfd,method1,len,0);
			pthread_t get;
			pthread_create(&get,NULL,ftp_get,port);
			break;
		case put:
			cout<<"put method"<<endl;
			len = strlen(method2);
			send(sockfd,method2,len,0);
			pthread_t put;
			pthread_create(&put,NULL,ftp_put,port);
			break;
		case quit:
			cout<<"quit process"<<endl;
			send(sockfd,"quit",4,0);
			exit(0);
			break;
		case pwd:
			cout<<"pwd method"<<endl;
			send(sockfd,"257",3,0);
			ftp_pwd(sockfd);
			break;
		default:
			break;
		}
	}
}

void control_process(void* arg)
{
	char buf[BUFFER_SIZE];
	memset(buf,'\0',BUFFER_SIZE-1);
	printf("enter the control_thread\n");
	if(1)
	{
		const char* login_name = "juyang";
		const char* passwd = "123456";
		int* fd = (int*)arg;
		cout<<*fd<<endl;
		
		const char* cmd = "220 (ftpd 0.0.1)";
		const char* cmd1 = "331 Please specify the password";
		const char* cmd2 = "230 Login successful.\nRemote system type is UNIX.\nUsing binary mode to transfer files.";

		int len = strlen(cmd);
		int len1 = strlen(cmd1);
		int len2 = strlen(cmd2);
		
		int ret = send(*fd,cmd,len,0);//send 220
		
		while(1)
		{
			sleep(4);	
			int recvRet = recv(*fd,buf,sizeof(buf),0);//recv name
			cout<<"username :> "<<buf<<endl;
			if(!(strcmp(buf,login_name)))
			{
				send(*fd,cmd1,len1,0);
				break;
			}
			else
				cout<<"false username Please input again!"<<endl;
		
		}

		while(1)
		{
			sleep(4);
			memset(buf,'\0',BUFFER_SIZE-1);
			recv(*fd,buf,sizeof(buf),0);
			pthread_t tid;
			if(!(strcmp(buf,passwd)))
			{
				cout<<"enter strcmp passwd"<<endl;
				sleep(1);
				send(*fd,cmd2,len2,0);
				cout<<"password :> "<<buf<<endl;
	
				break;
			}
			else
			{
				cout<<"false password,Please input again!"<<endl;
			}
		}
		sleep(1);
//		char str[5];
//		int randPort = 1027+rand()%100;
//		char* strPort = itoa(randPort,str); //server create a rand port
//		int portLen = strlen(strPort);	
//		
//		cout<<"randPort :>"<<strPort<<endl;
//		if(send(*fd,strPort,portLen,0) == -1)
//		{
//			cout<<"send failure"<<endl;
//		}
//		else
//		{
//			cout<<strPort<<endl;
//			cout<<"send success"<<endl;
//		}
		
		process_mnd(*fd);
//		pthread_create(&tid,NULL,connect_dataLink,fd);
		//close(*fd);		
	}
}

void accept_request(void* arg)
{
	int* fd = (int*)arg;
	cout<<"accept_request fd :> "<<*fd<<endl;
	pid_t pid;
	if(fork() == 0)//child
	{
		cout<<"child process"<<endl;
	
		control_process(fd);
		shutdown(*fd,2);
	
	}else{
		cout<<"father process"<<endl;
		int status = 0;
		pid_t ret = 0;
		do{
			ret = waitpid(-1,&status,WNOHANG);
			sleep(1);
		}while(ret == 0);
	}
}
/* block the fd*/
int setnonblocking(int fd)
{
	int old_option = fcntl(fd,F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd,F_SETFL,new_option);
	return old_option;
}
void addfd(int epollfd, int fd,bool enable_et)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN;
	if(enable_et)
	{
		event.events |= EPOLLET;
	}
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
	setnonblocking(fd);
}

void et(epoll_event* events,int number,int epollfd,int listenfd)
{
	epoll_event event;
	char buf[BUFFER_SIZE];
	for(int i=0; i<number;i++)
	{
		int sockfd = events[i].data.fd;
		if(sockfd == listenfd)
		{
			struct sockaddr_in client_address;
			socklen_t client_addrlength = sizeof(client_address);
			int connfd = accept(listenfd,(struct sockaddr*)&client_address, &client_addrlength);
	//		addfd(epollfd,connfd,true);//open et mode to connfd
			setnonblocking(connfd);
			event.data.fd = connfd;
			event.events = EPOLLIN | EPOLLET;
			epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event);
		}
		else if(events[i].events & EPOLLIN)
		{
			printf("event trigger once\n");
			while(1)
			{
				memset(buf,'\0',BUFFER_SIZE-1);
				int ret = recv(sockfd,buf,sizeof(buf),0);//recv hello server
				printf("recv hello server ret :> %d\n",ret);
				if(ret < 0)
				{
					if((errno == EAGAIN) || (errno == EWOULDBLOCK))
					{
						cout<<"read later"<<endl;
						break;
					}
					close(sockfd);
					break;
				}
				else if(ret == 0)
				{
					close(sockfd);
				}
				else
				{
					cout<<"get"<<" "<<ret<<" "<<"bytes of content : "<<buf<<endl;
					accept_request(&sockfd);
				}
			}
		}
		else
		{
			cout<<"somthing else happened"<<endl;
		}
	}
}
void processConnect_(int listenfd)
{
	assert(listenfd != -1);
	epoll_event events[MAX_EVENT_NUMBER];
	int epollfd = epoll_create(5);
	assert(epollfd != -1);
	addfd(epollfd,listenfd,true);//epoll_ctl

	while(1)
	{
		int ret = epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
		if(ret < 0)
		{
			cout<<"epoll failure"<<endl;
			break;
		}
		et(events,ret,epollfd,listenfd);
	}
	close(listenfd);
}
int ftpServer::initConnect()
{
	return start(_port);
}
void ftpServer::processConnect()
{
	int listenfd = initConnect();
	processConnect_(listenfd);
}
