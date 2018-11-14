#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
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

	fd_set readfds,clientfd;
	FD_ZERO(&readfds);
	FD_SET(sockfd,&readfds);	

	while(1)
	{
		clientfd=readfds;
		printf("server waiting\n");
		int result=select(FD_SETSIZE,&clientfd,(fd_set *)0,(fd_set *)0,(struct timeval *)0);
		if(result<1)
		{
			perror("server5");
			exit(1);
		}

		for(int fd=0;fd<FD_SETSIZE;fd++)
		{
			if(FD_ISSET(fd,&clientfd))
			{
				if(fd==sockfd)
				{
					forclientsockfd=accept(sockfd,(struct sockaddr *)&clientinfo,&addrlen);
					FD_SET(forclientsockfd,&readfds);
					printf("adding client on fd %d\n",forclientsockfd);
				}
				else
				{
					read(fd,receive,1024);
					write(fd,"HTTP/1.1 200 OK\r\n",strlen("HTTP/1.1 200 OK\r\n"));
					write(fd,"\r\n\r\n",strlen("\r\n\r\n"));
					write(fd,htmlcontent,htmllength);
					FD_CLR(fd,&readfds);
					close(fd);
					printf("removing clienton fd %d\n",fd);
				}
			}
		}
	}
	close(sockfd);
	return 0;
}



