#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define MAXLINE 8192


int main(int argc, char **argv){
	int clientfd;
	int port;
	char *host;
	char *id = argv[1];

	if(argc != 4){
		fprintf(stderr, "usage: %s <id> <host> <port>\n", argv[0]);
		exit(0);
	}

	printf("Creating socket.\n");
	clientfd = socket(AF_INET, SOCK_STREAM,0);

	struct sockaddr_in serveraddr;

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET,argv[3],&serveraddr.sin_addr);
	bzero((char *)&serveraddr.sin_zero, sizeof(serveraddr.sin_zero));

	printf("Client connecting...\n");

	int status = connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(status < 0){
		printf("Could not connect.\n");
		close(clientfd);
		exit(-1);
	}
	printf("Successfully connected.\n");

	char *hello = (char *) malloc(MAXLINE);
	sprintf(hello,"cs230 HELLO %s\n", id);
	status = send(clientfd,hello,strlen(hello),0);	
	if(status<0){
		printf("error occurred");
		exit(0);
	}
	char sub_cipher[26];
	for(int i=0;i<305;i++){
		char status_message[MAXLINE];
		char *message = (char *)malloc(MAXLINE);
		char *decrypted = (char *)malloc(MAXLINE);
		status = recv(clientfd, status_message, MAXLINE,0);
		sscanf(status_message,"cs230 STATUS %26s %8192s", sub_cipher, message);
		char *decrypted_message = (char*)malloc(MAXLINE);
		for(int j=0;message[j]!='\0';j++){
			char cur = message[j];
			int index = cur - 'a';
			decrypted_message[j] = sub_cipher[index];
		}
		
		sprintf(decrypted, "cs230 %s\n", decrypted_message);

		printf("%s\n", decrypted);

		status = send(clientfd, decrypted, strlen(decrypted),0);
		if(status<0){
			printf("error occurred");
			exit(0);
		}
		free(decrypted);
		free(message);
		free(decrypted_message);
	}
	
	char flag[64];
	recv(clientfd,flag,64,0);
	printf("Flag received: %s\n", flag);
	close(clientfd);
	exit(0);
}



