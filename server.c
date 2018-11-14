#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc , char *argv[])
{

	int htmlfd=0,htmllength=0;
	char htmlcontent[8192],receive[1024];

	htmlfd=open("ccu.html",O_RDONLY);
	htmllength=read(htmlfd,htmlcontent,8192);
	htmlcontent[htmllength]='\0';
	htmllength+=1;

	int sockfd=0,forclientsockfd=0,pid;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in serverinfo,clientinfo;
	int addrlen=sizeof(clientinfo);
	bzero(&serverinfo,sizeof(serverinfo));

	serverinfo.sin_family=PF_INET;
	serverinfo.sin_addr.s_addr=INADDR_ANY;
	serverinfo.sin_port=htons(8700);
	
	bind(sockfd,(struct sockaddr*)&serverinfo,sizeof(serverinfo));

	listen(sockfd,5);

	while(1)
	{
		printf("server waiting\n");
		forclientsockfd=accept(sockfd,(struct sockaddr*)&clientinfo,&addrlen);
		printf("adding clinet on fd %d\n",forclientsockfd);
		if((pid=fork())==0)
		{
			read(forclientsockfd,receive,1024);
			write(forclientsockfd,"HTTP/1.1 200 OK\r\n",strlen("HTTP/1.1 200 OK\r\n"));
			write(forclientsockfd,"\r\n\r\n",strlen("\r\n\r\n"));
			write(forclientsockfd,htmlcontent,htmllength);
			close(forclientsockfd);
			printf("removing client fd %d\n",forclientsockfd);
		}
		else
			break;
	}
	
	close(sockfd);
	return 0;
}
