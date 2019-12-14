/*	client.c	*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h> 


#define SIZE 1000000

int main()
{
	char input[4096];
	char url[4096];
	char index[4096];
	char *token;
	char *argv[1000];
	char temp[4096];
	
	int i = 0;
	int sockfd, n, port;
	
	struct sockaddr_in servaddr;

	/* get client <port_number>*/
	printf("Enter client port # following this format:\n") ;
	printf("client <port_number>\n");
	printf("client: ");
	fgets(input, sizeof(input), stdin);		// get input from user
	
	token = strtok(input, " ");		// parse the port number
	while(token != NULL)
	{
		argv[i] = token;
		token = strtok(NULL," ");
		++i;
	}

	
	port = strtol(argv[0], NULL, 10);	// convert string into integer

	

	sockfd = socket(AF_INET,SOCK_STREAM,0);	// intialize socket
	if(sockfd == -1)
	{
		printf("socket creation failed...\n");
		exit(0);
	}
	else {printf("socket successfully created...\n");}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;			// sin family
	servaddr.sin_addr.s_addr = inet_addr("129.120.151.94");
	servaddr.sin_port = htons(port);
	



	int conn_fd;	// connect variable

	if (conn_fd = connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)		// connect to client
		{
			printf("connection with server failed...\n");
			exit(1);
		}
		else {
			printf("connected to server...\n\n");
		}


	/* After successfully connected */

	/* get url */
	printf("-----------------------\n");
	bzero(temp,sizeof(temp));
	bzero(url,sizeof(url));
	
	printf("Enter request url following this format:\n") ;
	printf("url: ");
	
	fgets(temp,sizeof(temp),stdin);			// get input from user
	
	
	strcpy(url, temp);

	int write_fd;
	
	write_fd = write(sockfd, url, strlen(url)); // write to server
	
	char recvline[SIZE];

	bzero(recvline, sizeof(recvline));
	recv(sockfd, recvline, sizeof(recvline)-1, MSG_WAITALL);		// wait until receive everything from server

	close(conn_fd);
	printf("%s\n", recvline);		// print out on the screen
	
	close(sockfd);


	return 0;
}