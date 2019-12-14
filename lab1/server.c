/*	server.c	*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#define MAX 6000000
int main()
{
	
	char input[4096];		// input buffer
	char *token;			// tokenize 
	char *argv[1000];		// get the token
	
	int i = 0;
	
	printf("Enter proxy executable and port # following this format:\n") ;
	printf("pserver <port_number>\n");

	fgets(input, sizeof(input), stdin);		// get user input
	
	token = strtok(input, " ");			// tokenize user's input
	while(token != NULL)
	{
		argv[i] = token;
		token = strtok(NULL," ");
		++i;
	}
	
	if(strcmp(argv[0], "pserver")!=0)
	{
		printf("Please follow this format: pserver <port_number>\n");
		exit(1);
	}
	
	char port[10];
	sscanf(argv[1],"%[^\n]" ,port);		// parse the port 
	
	 int _port ; 	
	_port = strtol(argv[1], NULL, 10);	// convert string into integer
	

	char str[200];
	int listen_fd, conn_fd;
	struct sockaddr_in servaddr;	//create server address

	if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error\n");
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;    
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(_port); 

	int on = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));		// make sure there is not binding error


	/* Binds the above details to the socket */
	
	if ( bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0)
	{
		printf("socket bind failed...\n");
		exit(1);
	}
	else {	
		printf("Succesfully binded\n");}

	/* Listen to incoming connections*/
	if(listen(listen_fd, 10) != 0 )
	{
		printf("listen failed...\n");
		exit(1);
	}
	else {printf("server listening\n");}



	char recvline[10000], http_request[100000]; 
	char response[MAX] ;
	char *IPbuffer;
	int n, sockfd, s;
	char first_part[10000], second_part[1000];



	


		/* Accept incomming connections */
		conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
		n = read(conn_fd, recvline, sizeof(recvline));
		printf("Please wait while proxy fetching data...\n");
		/*get the first part of the string*/
		sscanf(recvline,"%[^/\n]",first_part);
		
		/*get the second part of the string*/
		sscanf(recvline,"%*[^/]/%[^\n]", second_part);

		/*check whether the url has /index or not */
		if(strcmp(second_part, " ")==0)
			sprintf(http_request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", " ", first_part);
		else
			sprintf(http_request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", second_part, first_part);
	
		
		
		/* create proxy server */
		struct addrinfo hints, *p,*result;		// proxy
		memset(&hints, 0, sizeof(hints));
		hints.ai_socktype=SOCK_STREAM;
		hints.ai_family=AF_UNSPEC;
		
		s = getaddrinfo(first_part, "80", &hints, &result);			// DNS lookups
		
		
		if(s!=0)
		{
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
			exit(EXIT_FAILURE);
		}
	
	
		for(p=result;p!=NULL;p=p->ai_next)
		{
			if (( sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)		// create a new socket to connect webserver
			{
				printf("socket error!\n");
				return 0;
			}
			
			if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)		// connect webserver
			{
				close(sockfd);
				perror("connect error!");
				continue;
			}
	
		}

		send(sockfd, http_request, strlen(http_request), 0);		// send request to webserver
		bzero(response, sizeof(response));		// clear buffer
		freeaddrinfo(result);	// free memory	
		recv(sockfd, response, MAX, MSG_WAITALL);			// wait until receive everything from webserver
		
			
		/* send the webpage source to client */
		send(conn_fd, response, strlen(response), 0);		


		close(sockfd);		// close socket
		close(listen_fd);	
		
		
	return 0;
}