#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <resolv.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <syslog.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/resource.h>

#define PORT 80
#define WWW_DIR "/var/www"
#define SIZE 1024
#define MAXPATH 100

void usage()	 
{
printf("Applied options:\n -p port\n -d directory contains html files\n -h help\n");
	exit(0);
}

void errors(int sock, int tok)	 
{
	static char *e[]={"HTTP/1.0 404 Not Found\n"};
	send(sock, e[tok], strlen(e[tok]), 0);
}

void sesHandler(int sock, char *www_dir)	 
{
	char file[MAXPATH];	
	char tmp[MAXPATH];	
	char buf[SIZE];		
	char head[SIZE];
	char *reply;
	FILE *fd;
	int i, j=0;
	struct stat filestat;

	bzero(&buf, sizeof(buf));
	bzero(&tmp, MAXPATH);
	bzero(&file, MAXPATH);
	recv(sock, buf, SIZE, 0);

	sscanf(buf, "GET %s HTTP", file);
	if(file[1]=='\0') strcat(file,"index.html");
	for(i=0; file[i]; i++){
		if(file[i]=='/')for(i; file[i]; i++){tmp[j++]=file[i];}
	}
	j=i=0;
	bzero(&file, MAXPATH);
	strcpy(file, www_dir);
	strcat(file, tmp);

	if((fd=fopen(file, "r"))==NULL){errors(sock, 0);}	 
	else{
		stat(file, &filestat);			 
		reply=(char*)malloc(filestat.st_size);
		bzero(&head, SIZE);			 
		sprintf(head, "HTTP/1.0 200 OK\n Server: Simple httpd\nContent-Legnth: %d\n \Content-type: text/html\n\n\n", filestat.st_size);
		send(sock, head, strlen(head), 0);
		while(fread(reply, filestat.st_size, 1, fd)){	
			send(sock, reply, filestat.st_size, 0);
			free(reply);
		}
	}
}
