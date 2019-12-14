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
#include <string.h>

#define SIZE 50000
#define FIN 	0b0000000000000001
#define SYN 	0b0000000000000010
#define ACK 	0b0000000000010000
#define SYN_ACK 0b0000000000010010

typedef struct  {                
	unsigned short int src;                
	unsigned short int des;                
	unsigned int seq;                
	unsigned int ack;                
	unsigned short int hdr_flags;                
	unsigned short int rec_window;                
	unsigned short int cksum;                
	unsigned short int ptr;                
	unsigned int opt;       
	unsigned char payload[128];       
}tcp_hdr;

unsigned int checksum(unsigned short int * arr);
unsigned short int flag_SYN( tcp_hdr * tcp_seg);
unsigned short int flag_FIN( tcp_hdr* tcp_seg);
unsigned short int flag_ACK( tcp_hdr* tcp_seg);
uint32_t generate_inital_seq();
uint32_t generate_inital_seq2();
void print_struct(tcp_hdr * tcp_seg);
char * data_load(char temp[], int tempIndex);
char * print_binary_flags(tcp_hdr * tcp_seg);
void output(FILE * writeFile, tcp_hdr * tcp_seg);

int main()
{
	// struct for packet header
	tcp_hdr tcp_seg;		// opening TCP connection
	tcp_hdr tcp_close;		// closing TCP connection				
	unsigned short int chksum_array[12];
	unsigned int cksum;		// check sum value
	
	unsigned short int header_length;
	unsigned short int temp_client_sequence;
	unsigned short int temp_server_sequence;


	// struct for socket
	struct sockaddr_in servaddr;		
	int conn_fd;	// connect variable
	int sockfd, n;
	int write_fd;
	char recvline[1000];
	char temp[1024];		// store the data from the file
	char buffer[128];		
	char *buff ;
	// write to file
	FILE * writeFile;
	FILE * payFile;

//--------------------------------------------------------------------------------
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
	servaddr.sin_port = htons(1500);
	

	if (conn_fd = connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)		// connect to client
		{
			printf("connection with server failed...\n");
			exit(1);
		}
		else {
			printf("connected to server...\n\n");
		}


	/* After successfully connected */

//--------------------------------------------------------------------------------
// send TCP request to server
	writeFile = fopen("client.out", "w");
	printf("Opening TCP connection...\n");
	fprintf(writeFile, "%s\n", "Opening TCP connection...");
	header_length = sizeof(tcp_seg);
	header_length = header_length << 11;	// shift 11 bits
	tcp_seg.hdr_flags = header_length;		// assign header legnth
	
	tcp_seg.src = 1500;
	tcp_seg.des = 1500;
	tcp_seg.seq = generate_inital_seq();	// sequence #
	tcp_seg.ack = 0;	// ack #
	tcp_seg.hdr_flags = flag_SYN(&tcp_seg);	// assign SYN
	tcp_seg.rec_window = 0;  
    tcp_seg.cksum = 0;  
    tcp_seg.ptr = 0;  
    tcp_seg.opt = 0;  
	
	memcpy(chksum_array, &tcp_seg, 24);
	cksum = checksum(chksum_array);
	tcp_seg.cksum = cksum;		// assign checksum to tcp segment

	temp_client_sequence = tcp_seg.seq;

	write_fd = write(sockfd,  (tcp_hdr*) &tcp_seg , sizeof(tcp_seg) ); // send SYN
	
	
//--------------------------------------------------------------------------------
// receive ACK from server
	printf("----Receive ACK and SYN----\n");
	n = read(sockfd, &tcp_seg ,sizeof(tcp_seg));		// receive ACK and SYN
	output(writeFile, &tcp_seg);		// write to client.out
	print_struct(&tcp_seg);

	

	tcp_seg.ack = tcp_seg.seq + 1;		// acknowledgement number equal to initial server sequence number + 1 			
	tcp_seg.seq = temp_client_sequence + 1;	// sequence number as initial client sequence number + 1 
	
	temp_client_sequence = tcp_seg.seq;		// temporarily save server sequence number
	temp_server_sequence = tcp_seg.ack;

	tcp_seg.hdr_flags = header_length;		// assign header legnth
	tcp_seg.hdr_flags = flag_SYN(&tcp_seg);	 // Set the ACK bit to 1

	tcp_seg.cksum = 0;		// reset cksum to 0
	memset(chksum_array, 0 , sizeof(chksum_array));		// clear array
	memcpy(chksum_array, &tcp_seg, 24);	// copy memory address to array
	cksum = checksum(chksum_array);		// compute checksum
	tcp_seg.cksum = cksum;		// assign checksum to tcp segment


	write_fd = write(sockfd,  (tcp_hdr*) &tcp_seg , sizeof(tcp_seg) ); // send ACK

//--------------------------------------------------------------------------------
//Start transferring file
	printf("\n");
	printf("----Sending data----\n\n");

	payFile = fopen("word.txt", "r");	// read word.txt 
	int c;
	int i = 1;
	int tempIndex = 0;	
	int bufferIndex = 0;
	int k = 0;

	while((c = fgetc(payFile)) != EOF)	// read until end of file
	{
		temp[bufferIndex] = c;
		bufferIndex++;
	}
	temp[bufferIndex] = '\0';		

	fclose(payFile);
	

	tcp_seg.hdr_flags = header_length;		// assign header legnth
	tcp_seg.hdr_flags = flag_ACK(&tcp_seg); // set the ACK bit to 1 


	while(k < 8)
	{

		strcpy(tcp_seg.payload, data_load(temp, tempIndex));		// copy the text file into buffer

		
		write_fd = write(sockfd,  (tcp_hdr*) &tcp_seg , sizeof(tcp_seg) ); // send data




		n = read(sockfd, &tcp_seg ,sizeof(tcp_seg));		// read data	
		printf("Segment %d: FILE TRANSFER\n",i);
		print_struct(&tcp_seg);
	
		// printf("ack number: %d\n", tcp_seg.ack);
		// printf("client sequence number: %d\n", tcp_seg.seq);

		fprintf(writeFile, "%s%d%s\n", "Segment ", i, ": FILE TRANSFER");
		output(writeFile, &tcp_seg);		// write to client.out


		temp_client_sequence = tcp_seg.ack;	// keep track of current server sequence
		temp_server_sequence = tcp_seg.seq;	// keep track of current client sequence

		tcp_seg.seq = temp_client_sequence + 1;		// increment server sequence number by 1
		tcp_seg.ack = temp_server_sequence + 1;	// increment 128 everytime send 128 bytes

		 

		tcp_seg.cksum = 0;		// reset cksum to 0
	
		memcpy(chksum_array, &tcp_seg, 24);	// copy memory address to array
		cksum = checksum(chksum_array);		// compute checksum
		tcp_seg.cksum = cksum;		// assign checksum to tcp segment


		
		tempIndex += 128;		// stop at 128th index everytime read the file
		k++;
		i++;
	}



	printf("\n");




//--------------------------------------------------------------------------------
	printf("Closing TCP connection...\n");
	fprintf(writeFile, "%s\n", "Closing TCP connection...");
	tcp_close.src = 1500;
	tcp_close.des = 1500;
	tcp_close.seq = generate_inital_seq2();		// Assign an initial client sequence number
	tcp_close.ack = 0;			// 0 ack number

	tcp_close.hdr_flags = header_length;		// assign header legnth		
	tcp_close.hdr_flags = flag_FIN(&tcp_close);		// set FIN bit to 1
	tcp_close.rec_window = 0;    
    tcp_close.opt = 0; 
	tcp_close.ptr = 0;  

	tcp_close.cksum = 0;	

	memset(chksum_array, 0 , sizeof(chksum_array));		// clear array
	memcpy(chksum_array, &tcp_close, 24);	// copy memory address to array
	cksum = checksum(chksum_array);		// compute checksum
	tcp_close.cksum = cksum;		// assign checksum to tcp segment


	temp_client_sequence = tcp_close.seq;	// temporarily save the initial sequence number

	write_fd = write(sockfd,  (tcp_hdr*) &tcp_close , sizeof(tcp_close) ); // send FIN


//--------------------------------------------------------------------------------

	printf("----First Receive ACK----\n");
	n = read(sockfd, &tcp_close ,sizeof(tcp_close));		// first receive
	output(writeFile, &tcp_close);		// write to client.out
	 print_struct(&tcp_close);
	
	printf("----Second Receive FIN----\n");

	n = read(sockfd, &tcp_close ,sizeof(tcp_close));		// second receive
	output(writeFile, &tcp_close);		// write to client.out
	print_struct(&tcp_close);
	
//--------------------------------------------------------------------------------
// client responds back with an acknowledgement TCP segment

	tcp_close.ack = tcp_close.seq + 1;	//  ack number equal to initial server sequence number + 1 
	tcp_close.seq = temp_client_sequence + 1;	// Assign a sequence number as initial client sequence number + 1
	
	tcp_close.hdr_flags = header_length;		// assign header legnth

	tcp_close.hdr_flags = flag_ACK(&tcp_close);		// set ACK bit to 1

	tcp_close.cksum = 0;	// reset cksum to 0

	memset(chksum_array, 0 , sizeof(chksum_array));		// clear array
	memcpy(chksum_array, &tcp_close, 24);	// copy memory address to array
	cksum = checksum(chksum_array);		// compute checksum
	tcp_close.cksum = cksum;		// assign checksum to tcp segment
	
	


	write_fd = write(sockfd,  (tcp_hdr*) &tcp_close , sizeof(tcp_close) ); // send ACK to server

	fclose(writeFile);
	close(conn_fd);
	close(sockfd);




	return 0;
}

unsigned int checksum(unsigned short int * arr)		// Compute sum   
{
	int i;
	unsigned short int sum = 0 , cksum = 0;
	for (i=0;i<12;i++)   
    {                             
      sum = sum + arr[i];  
    }
	cksum = sum >> 16;              // Fold once  
    sum = sum & 0x0000FFFF;   
    sum = cksum + sum;  
    cksum = sum >> 16;             // Fold once more  
    sum = sum & 0x0000FFFF;  
    cksum = cksum + sum;  /* XOR the sum for checksum */
	
	cksum = 0xFFFF^cksum;	// check sum
	return cksum;
}

uint32_t generate_inital_seq()
{
	return rand() % 10000 + 100;
}

uint32_t generate_inital_seq2()
{
	return rand() % 100 + 100;
}

unsigned short int flag_SYN( tcp_hdr* tcp_seg)
{
	tcp_seg->hdr_flags = tcp_seg->hdr_flags | SYN;		// assing 1 bit
	
	return tcp_seg->hdr_flags;
}

unsigned short int flag_FIN( tcp_hdr* tcp_seg)
{
	tcp_seg->hdr_flags = tcp_seg->hdr_flags | FIN;		// assing 1 bit
	
	return tcp_seg->hdr_flags;
}

unsigned short int flag_ACK( tcp_hdr* tcp_seg)
{
	tcp_seg->hdr_flags = tcp_seg->hdr_flags | ACK;		// assing 1 bit
	
	return tcp_seg->hdr_flags;
}

char * print_binary_flags(tcp_hdr * tcp_seg)		// represent flags in form of binary
{
	int mask;
	static char temp [7];
  	int j = 0;
	for (int i = 15 ; i >= 0; i--)
	{ 
		mask = tcp_seg->hdr_flags >> i;

		if (mask & 1)
			temp[j] = '1'; 
		else 
			temp[j] = '0';
		j++;		
	}
  return temp;
}

char * data_load(char temp[], int tempIndex)		// get the data from text file
{
	int bufferIndex = 0;
	static char buffer[128];
	
	while(bufferIndex < 128)
	{
		buffer[bufferIndex] = temp[tempIndex];

		bufferIndex++;
		tempIndex++;
	}
	buffer[bufferIndex] = '\0';
	// printf("%s", buffer);

	return buffer;
}





void print_struct(tcp_hdr * tcp_seg)
{
	printf("src:\t\t%d\n", tcp_seg->src);
	printf("des:\t\t%d\n", tcp_seg->des);
	printf("seq:\t\t%d\n", tcp_seg->seq);
	printf("ack:\t\t%d\n", tcp_seg->ack);
	printf("hdr_flags:\t%s\n", print_binary_flags(tcp_seg));
	printf("rec_window:\t%d\n", tcp_seg->rec_window);
	printf("chksum:\t\t%04X\n", tcp_seg->cksum);
	printf("ptr:\t\t%d\n", tcp_seg->ptr);
	printf("opt:\t\t%d\n", tcp_seg->opt);
	printf("--------------------------------------------------------\n");
}

void output(FILE * writeFile, tcp_hdr * tcp_seg)
{
	fprintf(writeFile,"%s:\t\t%d\n", "src", tcp_seg->src);
	fprintf(writeFile,"%s:\t\t%d\n", "des", tcp_seg->des);
	fprintf(writeFile,"%s:\t\t%d\n", "seq", tcp_seg->seq);
	fprintf(writeFile,"%s:\t\t%d\n", "ack", tcp_seg->ack);
	fprintf(writeFile,"%s:\t%s\n", "hdr_flags", print_binary_flags(tcp_seg));
	fprintf(writeFile,"%s:\t%d\n", "rec_window", tcp_seg->rec_window);
	fprintf(writeFile,"%s:\t\t%04X\n", "chksum", tcp_seg->cksum);
	fprintf(writeFile,"%s:\t\t%d\n", "ptr", tcp_seg->ptr);
	fprintf(writeFile,"%s:\t\t%d\n", "opt", tcp_seg->opt);
  	fprintf(writeFile,"%s\n", "--------------------------------------------------------");
}