#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<pwd.h>
#include"ls.h"
#include<sys/sendfile.h>
#include<fcntl.h>
#include<sys/stat.h>


void Thread_handler(void *arg)
{
	int *sock=(int *)arg;
	int sock_desc=*sock;
	int bytes,flag=0,access,bytes_read;
	char message[256],tmp_msg[256],command[256],path[256],path1[256],usernames[5][3]={"u1","u2","u3","u4","u5"},response[256]={"Failure"};
	char success[256]={"Success"},username[256],ls_fail[256]={"Permission denied : You don't have permission to access this Directory!\n"};
	char dir_res[256]={"Direcorty Doesn't Exist!\n"};
	bytes=recv(sock_desc,message,256,0);

	DIR *mydir;
	struct dirent *myfile;
	struct stat mystat;
	FILE *fp1,*fp2;
	char *line,c,str[256];
	size_t len;
	ssize_t read;
	int i=0;
	

	for(i=0;i<5;i++)
	{
		if((strcmp(message,usernames[i]))==0)
			flag=1;
	}
	if(flag==0)
	{
		send(sock_desc,response,sizeof(response),0);
		close(sock_desc);
		pthread_exit(NULL);
	}
	send(sock_desc,success,256,0);
	char * argu;
	char delim[]={" "};
	snprintf(path1,256,"/home/simple_home/%s/",message);
	strcpy(username,message);
	strcat(username,"\n");
	chdir(path1);
	bytes=1;
	while(bytes)
        {
                bytes=recv(sock_desc,message,256,0);
		strcpy(tmp_msg,message);
		argu=strtok(tmp_msg,delim);
		
		if(strcmp(argu,"ls")==0)
		{
			/* ls Functinality Code*/
			char *argum;
			argum=message+3;
			if((chdir(argum)==-1))
			{
				send(sock_desc,dir_res,strlen(dir_res),0);
				continue;
			}
			FILE *al_users;
			al_users=fopen("allowed_users.txt","r");
			access=0;
			line=NULL;
			len=0;
			while((bytes_read=getline(&line,&len,al_users))!=-1)
			{	
				printf("%s\n",line);
				if(strcmp(line,username)==0)
				{
					printf("Access Granted\n");
					//chdir(path1);
					access=1;
					break;
				}
			}
			fclose(al_users);
			if(access==0)
			{
				printf("Access Denied\n");
				chdir(path1);
				send(sock_desc,ls_fail,strlen(ls_fail),0);
				continue;
			}


			fp1=fopen("dir_info.txt","r");
         		fp2=fopen("ls_output.txt","w");
    			//char buf[512];
			//argum=message+3;
			printf("Just Before opendir() and readdir()\n");
    			if((mydir = opendir("."))==NULL)
			{
				printf("opendir error\n");
			}
			len=0;
			line=NULL;
    			while((myfile = readdir(mydir)) != NULL)
    			{
        			//sprintf(buf, "%s/%s", argum, myfile->d_name);
        			rewind(fp1);
        			while ((read = getline(&line, &len, fp1)) != -1) 
				{
                			c=line[0];
                			while (c!=' ')
                			{
                	        		str[i++]=c;
                	        		c= line[i];
                			}
                			str[i]='\0';
                			if((strcmp(str,myfile->d_name))==0)
                			{
                	        		fwrite(line,strlen(line),1,fp2);
                			}
                			strcpy(str,"\0");
                			i=0;
        			}
    			}
    			fclose(fp1);
    			fclose(fp2);
    			closedir(mydir);

		

			/* Open the file that we wish to transfer */
        		FILE *fp = fopen("ls_output.txt","rb");
        		if(fp==NULL)
        		{
            			printf("File opern error");
            			exit(-1);
        		}
                        fseek(fp, 0L, SEEK_END);
			int sz = ftell(fp);
			rewind(fp);
			//fclose(fp);
			if(sz)
			{
				/* Read data from file and send it */
        			while(1)
        			{
            				/* First read file in chunks of 256 bytes */
            				unsigned char buff[256]={0};
            				int nread = fread(buff,1,256,fp);
            				printf("Bytes read %d \n", nread);
	
	        	        	/* If read was success, send data. */
            				if(nread > 0)
            				{
                				printf("Sending \n");
                				send(sock_desc, buff, nread,0);
            				}
		
            				/*
             				* There is something tricky going on with read ..
             				* Either there was error, or we reached end of file.
             				*/
            				if (nread < 256)
            				{
                				if (feof(fp))
         		        	   		printf("End of file\n");
                				if (ferror(fp))
                	    				printf("Error reading\n");
                				break;
            				}


        			}
			}
			else
			{
				send(sock_desc,"Directory Empty!!\n", 19,0);
			}
			fclose(fp);
			chdir(path1);
		}

		else if(strcmp(argu,"cd")==0)
		{
			
			 char *argum;
                         argum=message+3;
			 printf("\n%s\n",argum);
                         if((chdir(argum)==-1))
                         {
                                 send(sock_desc,dir_res,strlen(dir_res),0);
                                 continue;
                         }
                         FILE *al_users;
                         al_users=fopen("allowed_users.txt","r");
                         access=0;
                         line=NULL;
                         len=0;
                         while((bytes_read=getline(&line,&len,al_users))!=-1)
                         {
                                 if(strcmp(line,username)==0)
                                 {
                                         printf("Directory changed\n");
					 strcat(path1,argum);
					 send(sock_desc,"Directory Changed\n",18,0);
                                         access=1;
                                         break;
                                 }
                         }
                         fclose(al_users);
                         if(access==0)
                         {
                                 printf("Access Denied\n");
                                 chdir(path1);
                                 send(sock_desc,ls_fail,strlen(ls_fail),0);
                                 continue;
                         }
		}
		else if(strcmp(argu,"fput")==0)
		{
			char *argum,*file_name,*user_data,*group,*data;
			argum=message+5;
			file_name=strtok(argum," ");
			user_data=strtok(NULL," ");
			group=strtok(NULL," ");
			if(group==NULL)
			{
				FILE *fp=fopen(file_name,"a+");
				fputs(user_data,fp);
				fclose(fp);
				send(sock_desc,"Data Appended to file!!\n",25,0);
			}
			else
			{
				data=strtok(NULL," ");
				FILE *fp=fopen(file_name,"w");
				fputs(data,fp);
				fclose(fp);
				fp=fopen("dir_info.txt","a+");
				char entry[50];
				strcpy(entry,file_name);
				strcat(entry," ");
				strcat(entry,user_data);
				strcat(entry," ");
				strcat(entry,group);
				strcat(entry,"\n");
				fputs(entry,fp);
				fclose(fp);
				send(sock_desc,"New file successfully created\n",31,0);
			}
			
		}
		else if(strcmp(argu,"fget")==0)
		{
			char *argum=message+5;
			FILE *fp = fopen(argum,"rb");
			if(fp==NULL)
			{
				printf("File opern error");
			        exit(-1);
			}
			fseek(fp, 0L, SEEK_END);
			int sz = ftell(fp);
			rewind(fp);
			if(sz)
			{
				/* Read data from file and send it */
				while(1)
				{
					/* First read file in chunks of 256 bytes */
			        	unsigned char buff[256]={0};
			        	int nread = fread(buff,1,256,fp);
			        	printf("Bytes read %d \n", nread);
			        	/* If read was success, send data. */
			        	if(nread > 0)
			        	{
					        printf("Sending \n"); 
			        	        send(sock_desc, buff, nread,0);
			        	}      
			        	/*
			        	 * There is something tricky going on with read ..
			        	 * Either there was error, or we reached end of file.
			        	 */ 
			        	if (nread < 256)
			        	{
					        if (feof(fp))
					                printf("End of file\n");
			        	        if (ferror(fp))
			        	                printf("Error reading\n");
			        	        break;
			        	}
				}
			}
			else
			{
				send(sock_desc,"File is Empty!!\n",17,0);
			}
			fclose(fp);
		}
		else if(strcmp(argu,"create_dir")==0)
		{
			char *argum,*user,*group,*dir;
			argum=message+11;
			dir=strtok(argum," ");
			user=strtok(NULL," ");
                        //fflush(stdout);
			//printf("%s\n",dir);
			if(user==NULL)
			{
				mkdir(dir,(mode_t)022);
				chdir(dir);
				FILE *fp2=fopen("../allowed_users.txt","r");
			        fseek(fp2, 0L, SEEK_END);
			        int sz = ftell(fp2);
			        rewind(fp2);
			        fclose(fp2);
			        int fp=open("../allowed_users.txt",O_RDONLY);
			        int fp1=open("allowed_users.txt",O_CREAT | O_WRONLY,0644);
			        sendfile(fp1,fp,NULL,sz);
			        close(fp);
			        close(fp1);
				FILE *fp3=fopen("dir_info.txt","w");
				fclose(fp3);
				fp3=fopen("../owner.txt","r");
				char own[7];
				fgets(own,7,fp3);
				fseek(fp3,0L,SEEK_END);
				sz=ftell(fp3);
				printf("%d\n",sz);
				rewind(fp3);
				fclose(fp3);
				fp=open("../owner.txt",O_RDONLY);
				fp1=open("owner.txt",O_CREAT | O_WRONLY,0644);
				sendfile(fp1,fp,NULL,sz);
				chdir("../");
				close(fp);
				close(fp1);
				fp2=fopen("dir_info.txt","a+");
				char entry[50];
				strcpy(entry,dir);
				strcat(entry," ");
				strcat(entry,own);
				fputs(entry,fp2);
				fclose(fp2);
				send(sock_desc,"Directory Created\n",18,0);
			}
			else
			{
				FILE *fp;
				char path[50]={"/home/simple_home/"};
				printf("%s\n",user);
				mkdir(dir,(mode_t)022);
				chdir(dir);
				group=strtok(NULL," ");
				//printf("%s\n",group);
				printf("Hello\n");
				printf("%s\n",group);
				strcat(path,group);
				printf("%s\n",path);
				fp=fopen(path,"r");
				fseek(fp,0L,SEEK_END);
				int sz=ftell(fp);
				printf("%d\n",sz);
				rewind(fp);
				fclose(fp);
				int fp1=open(path,O_RDONLY);
				int fp2=open("allowed_users.txt",O_WRONLY|O_CREAT,0644);
				sendfile(fp2,fp1,NULL,sz);
				close(fp1);
				close(fp2);
				fp=fopen("allowed_users.txt","a+");
				fputs(user,fp);
				fclose(fp);
				fp=fopen("dir_info.txt","w");
				fclose(fp);
				fp=fopen("../owner.txt","r");
				char own[7];
				fgets(own,7,fp);
				fseek(fp,0L,SEEK_END);
				sz=ftell(fp);
				printf("%d\n",sz);
				rewind(fp);
				fclose(fp);
				fp1=open("../owner.txt",O_RDONLY);
				fp2=open("owner.txt",O_CREAT | O_WRONLY,0644);
				sendfile(fp2,fp1,NULL,sz);

				chdir("../");
				strcpy(path,dir);
				strcat(path," ");
				strcat(path,user);
				strcat(path," ");
				strcat(path,group);
				strcat(path,"\n");
				fp=fopen("dir_info.txt","a+");
				fputs(path,fp);
				fclose(fp);
				send(sock_desc,"Directory Created\n",18,0);
			}
		}
		else if(strcmp(argu,"newline")==0)
		{
			send(sock_desc,"\n",2,0);
		}
		else
		{
			char str[]={"Available Commands : \nls <directory_path>>\ncd <directory_path>\ncreate_dir <directory_path> <owner> <group>\nfput <filename> <user> <group> <data>\nfget <filename>\n"};
			send(sock_desc,str,strlen(str),0);	
		}
               //send(sock_desc,message,256,0);
        }

        close(sock_desc);

}

void main()
{
	int sock,sock_client,bytes=1,len,i;
//	char message[100];
	struct sockaddr_in client,server;
	pthread_t Threads[20];

	len=sizeof(struct sockaddr_in);
	server.sin_family=AF_INET;
	server.sin_port=htons(20001);
	server.sin_addr.s_addr=INADDR_ANY;
	bzero(&server.sin_zero,8);

	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1)
	{
		perror("Socket :");
		exit(0);
	}
	if((bind(sock,(struct sockaddr *)&server,len))==-1)
	{
		perror("Bind :");
		exit(0);
	}
	if((listen(sock,10)==-1))
	{
		perror("Listen :");
		exit(0);
	}
	for(;;)
	{
		
		if((sock_client=accept(sock,(struct sockaddr *)&client,&len))==-1)
		{
			perror("Accept :");
			exit(0);
		}
		//pthread_create(&Threads[i],NULL,Thread_handler,&sock_client);
		pid_t pid=fork();
		if(pid<0)
		{
			perror("error on fork");
			exit(1);
		}
		else if(pid==0)
		{
			close(sock);
			Thread_handler(&sock_client);
			exit(0);
		}
		else
		{
			close(sock_client);
		}
		
	}


}
