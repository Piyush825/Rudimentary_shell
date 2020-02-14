#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define RED  "\x1B[31m"
#define RESET "\x1B[0m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"

void main()
{
	struct sockaddr_in server;
	int sock,bytes,write_bytes,i;
	char line[256],recv_line[256],username[256],c;

	server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");
	server.sin_port=htons(20001);

	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("Socket :");
		exit(-1);
	}
	if((connect(sock,(struct sockaddr *)&server,sizeof(server)))==-1)
	{
		perror("Connect :");
		exit(-1);
	}
	printf("Enter your Username for Authentication :");
	fgets(username,256,stdin);
	username[strlen(username)-1]='\0';
	send(sock,username,256,0);
	recv(sock,recv_line,256,0);
	if(!strcmp(recv_line,"Success"))
		printf("Authentication Successful!!\n");
	else
	{
		printf("Wrong Credentials!! \n Exiting now....\n");
		exit(-1);
	}
	while(1)
	{
		FILE *fp;
		printf(BLU ">>>>" RESET);
		fgets(line,256,stdin);
		if(line[0]=='\n')
		{
			send(sock,"newline",8,0);
			goto write_data;
		}
		line[strlen(line)-1]='\0';
		if((strcmp(line,"exit"))==0)
		{
			printf(RED "Exiting!!\n" RESET);
			exit(0);
		}
		i=0;
		if((bytes=send(sock,line,256,0))==-1)
		{
			perror("send :");
		}
		write_data:
    		fp = fopen("abcd.txt", "ab");
    		if(fp == NULL)
    		{		
    		        printf("Error opening file");
    		    	exit(-1);
    		}
   		 /* Receive data in chunks of 256 bytes */
		bytes=1;
		while(bytes > 0)
   		{
			bytes = recv(sock, recv_line, 256,0);
        		write_bytes=fwrite(recv_line, 1,bytes,fp);
			if(bytes<256)
				break;
    		}
		fclose(fp);
		system("cat abcd.txt");
		system("rm abcd.txt");
	}
	close(sock);

}
