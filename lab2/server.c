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
#include <time.h>
#include <stdbool.h> 

#define MAX 6000000

char *calcTime();


int main()
{
	
	char input[20];		// input buffer
	char *token;			// tokenize 
	char *argv[1000];		// get the token

	char *token2;
	char *argv2[1000];
	int i = 0;


	printf("Enter proxy executable and port # following this format:\n") ;
	
	
	printf("pserver <port_number>\n");
	printf("pserver: ");

	fgets(input, sizeof(input), stdin);		// get user input
	

	sscanf(input,"%[^\n]" ,input);		// parse the port 
	
	 int _port ; 	
	_port = strtol(input, NULL, 10);	// convert string into integer

	

	/* establish server connection */

	int listen_fd, conn_fd;
	struct sockaddr_in servaddr, cli_addr;	//create server address

	if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error\n");
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;    
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(_port); 		//_port

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



	char recvline[10000], http_request[1000], http_badRequest[1000]; 
	char response[MAX];
	
	int n, sockfd, s;
	char first_part[10000], second_part[1000];
	
	char * myTime;	// get local time
	char * blockTime;
	unsigned long long _blockTime;

	// file initialization
	FILE * http_file;
	FILE * cache_file;
	FILE * blocked_file;
	
	char list[10000];
	char cache_filename[1000];
	char OK[100000];
	
	//check whether the url has been blocked
	char * blocking;
	char * token3;		// token for blocking
	char * blockToken[1000];
	char tempBlock[1000];		// temporary save received blocked url

	char blockURL[1000];		// store blocking URL
	char startTime[100000];
	char endTime[100000];
	unsigned long long _startTime, _endTime;

	char blockMSG[] = "Website Blocked!";

	// check whether the cache file is already existed
	char *reading;
	char * token4;		// token for updating
	char *storeURL[1000];

	char cacheURL[5][10000];
	char cacheNumber[5][10000];
	char temp[1000];		// temporary save received url
	char sendingCache[100000];
	char c;

	// file reading and index for parsing
	int file_length = 0;
	int blockFile_length = 0;
	int a = 0;
	int b = 0;
	int d = -1;
	int e = 0;


	// updating list
	
	char buffer[5][100000];	// store most current url
	char ignore[1][1000];		// to delete oldest url
	char *delete;
	int z, count = 0;
	int buffIndex = 0;
	int tempCount, index = 0;
	
	
		/* Accept incomming connections */

	http_file = fopen("list.txt","w");		//create a list
	
	
	while(1)
	{
		
		printf("---------------\n");	
		printf("Waiting for incoming request....\n");
		conn_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
		
		bzero(recvline, sizeof(recvline));
		n = read(conn_fd, recvline, sizeof(recvline));

		// check whether url has been blocked //
		blockTime = calcTime();		// time at recevied 
		_blockTime = strtoull(blockTime, NULL, 10);	
		
		bool foundBlocked = false;	// not found blocked url or not within the time
		strcpy(tempBlock , recvline);		// temporary store the url to check if it's in the list or not
		sscanf(tempBlock ,"%[^\n]", tempBlock); 	// get rid of new line character

		// printf("current tempBlock:%s\n", tempBlock);

		blocked_file = fopen("blacklist.txt", "r");
		if(blocked_file == NULL) {
			perror("Unable to open blacklist!");
			exit(1);
		}

		b = 0;
		while((getline(&blocking, &blockFile_length, blocked_file)) > 0)		// read line by line in blacklist
		{
			foundBlocked = false;
			a = 0;
			token3 = strtok(blocking, " ");
			while(token3 != NULL)
			{
				blockToken[a] = token3;
				token3 = strtok(NULL, " ");
				a++;
			}
			strcpy(blockURL, blockToken[0]);
		
			if(strcmp(blockURL, tempBlock) != 0)	// if not found blocked URL
			{
				continue;
				
			}

			else{			// if found blocked URl
				strcpy(startTime, blockToken[1]);
				strcpy(endTime, blockToken[2]);
				sscanf(endTime, "%[^\n]", endTime);

				_startTime = strtoull(startTime, NULL, 10);	// convert string to integer
				_endTime = strtoull(endTime, NULL, 10);

				if(_blockTime >= _startTime && _blockTime <= _endTime)		// if request time within the block time then true, else false
				{		
					foundBlocked = true;
				}
				 else {		
				 	foundBlocked = false;
				 }
			
				break;
			}			
		}
		fclose(blocked_file);


		if(foundBlocked == true)	// found blocked url within the block time
		{
			printf("Blocked URL found!\n");
			send(conn_fd, blockMSG, sizeof(blockMSG), 0);
			close(conn_fd); //close connection
		}

		else
	{


		//	check if url has been cached //	

		strcpy(temp , recvline);		// temporary store the url to check if it's in the list or not
		sscanf(temp ,"%[^\n]", temp); 	// get rid of new line character

		http_file = fopen("list.txt", "r");
		if(http_file == NULL) {
			perror("Unable to open list!");
			exit(1);
		}		
		
		b = 0;	
		bool found = false;  // url not found in the list
		while( (getline(&reading, &file_length, http_file)) > 0)	// read the file
		{ 		
			a = 0;	
			token4 = strtok(reading, " ");
			while(token4 != NULL)
			{			
				storeURL[a] = token4;
				token4 = strtok(NULL, " ");
				a++;
			}
			strcpy(cacheURL[b], storeURL[0]);		// store url
			strcpy(cacheNumber[b], storeURL[1]);	// store file name
			sscanf(cacheNumber[b] ,"%[^\n]", cacheNumber[b]);
			b++;
		
		}
		fclose(http_file);

		d = -1;		// keep track of cacheURL index
		
			for(int c = 0; c < 5; c++)
			{
				if(strcmp(cacheURL[c], temp) == 0)
				{
					found = true;		// true  -- found the url 
					d++;
					break;
				}
			}
		

		if(found == true)		// found the already cached files
		{	
			e = 0;
			printf("Cache file founded! Sending cache file to client...\n");
			
			cache_file = fopen(cacheNumber[d],"r");
			while( ( c = fgetc(cache_file)) != EOF)
			{
				sendingCache[e++] = c;
			}	
			fclose(cache_file);
			send(conn_fd, sendingCache, sizeof(sendingCache), 0);
			close(conn_fd);
		}
			
		else		// file has not been cached //
		{
			
			myTime = calcTime();	// time at recevied
			strcpy(list, recvline);		// copying url into list
			sscanf(list ,"%[^\n]", list); // get rid of new line character
			
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
		
			
			/////////* create proxy server *////////////

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

			memset(&response, 0, sizeof(response));
			send(sockfd, http_request, strlen(http_request), 0);		// send request to webserver
			
			freeaddrinfo(result);	// free memory	
			recv(sockfd, response, MAX, MSG_WAITALL);			// wait until receive everything from webserver
			
			

			/* check whether response is 200 OK */
			strcpy(OK, response);
			token = strtok(OK, " ");
			while(token != NULL)
			{
				argv[i] = token;
				token = strtok(NULL, " ");
				++i;
			}


			//* cache the webpage *//
			if(strcmp(argv[1], "200") == 0)
			{
				count++;	

				if( count <= 5)		// first 5 url
				{		
					/* storing the html webpage */
					printf("Cached the webpage...\n");

					sprintf(cache_filename, "%s", myTime);
					cache_file = fopen (cache_filename, "w");
					fputs(response, cache_file);
					fclose(cache_file);
				

					/* list.txt storing the url */
				
					http_file = fopen("list.txt","a+");	// create or open a file
					sprintf(list, "%s %s\n", list, myTime);	// concatenate url and time
					fputs(list, http_file);
					

					if(tempCount == 0)		// temporary save the first url
					{
						strcpy(ignore[0], list);
						tempCount++;
					}			
			
					strcpy(buffer[buffIndex], list);	
					buffIndex++;
						
					
					if(buffIndex == 5) {
						buffIndex = 0; // reset buffIndex
					}			
					
					fclose(http_file);	// save file	
				
				}	
			
					else 		// after first 5 url stored
					{
						// storing the html webpage
						printf("Cached the webpage...\n");
						
						sprintf(cache_filename, "%s", myTime);
						cache_file = fopen (cache_filename, "w");
						fputs(response, cache_file);
						fclose(cache_file);
						

						http_file = fopen("list.txt", "w+");
						sprintf(list, "%s %s\n", list, myTime);	// concatenate url and time	
							
					
					// update and delete oldest cache file	//

						if(index == 0)		// delete the oldest cache after first pass
						{
							z = 0;
							token2 = strtok(ignore[0], " ");
							while(token2 != NULL)
							{
								argv2[z] = token2;
								token2 = strtok(NULL, " ");
								++z;
							}
						
							sscanf(argv2[1] ,"%[^\n]", argv2[1]); // get rid of new line character
							remove(argv2[1]);	// remove oldest file
							index++;
							
						}
						else{		// delete the oldest cache as the program continues
							z = 0;		
												
							bzero(ignore, sizeof(ignore));

							delete = buffer[buffIndex];		// temporary save in delete pointer

							strcpy(ignore[0], delete);


							token2 = strtok(ignore[0], " ");
							while(token2 != NULL)
							{
								argv2[z] = token2;
								token2 = strtok(NULL, " ");
								++z;
							}
						
							sscanf(argv2[1] ,"%[^\n]", argv2[1]); // get rid of new line character
							remove(argv2[1]);	// remove oldest file

						
						}

						
						//	update the url list	//

						if(buffIndex == 5) {buffIndex = 0;}		// reset buffIndex

						strcpy(buffer[buffIndex], list);
					
						buffIndex++;	


						for(int k = 0; k < 5; k++)
						{
							fputs(buffer[k], http_file);	// write into file	
						}
						
				}

						fclose(http_file);

					/* send the webpage source to client */
					send(conn_fd, response, strlen(response), 0);	// send html webpage
					close(conn_fd);		// close connection
				
				} 
					// bad url request - return http response //
					else if(strcmp(argv[1], "200") != 0)
					{
						sprintf(http_badRequest,"HTTP response: %s\n", argv[1]);
						send(conn_fd, http_badRequest, strlen(http_badRequest), 0);	//send http request
						close(conn_fd);		// close connection
					}

			}
			
		}	
	}	

			close(sockfd);		// close socket
			close(listen_fd);
	
			
	return 0;
}


char *calcTime()
{
	time_t t;
	struct tm *myTime; 
	static char Time_Of_Visit[100];

	time(&t);		

	myTime = localtime(&t);		// get localtime

	strftime(Time_Of_Visit, sizeof(Time_Of_Visit), "%Y%m\%d%H%M\%S", myTime);		// time fomrat YYMMDDhhmmss

	return Time_Of_Visit;
}
