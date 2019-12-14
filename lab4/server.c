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

#define SIZE 50000
#define FIN 	0b0000000000000001
#define SYN 	0b0000000000000010
#define ACK 	0b0000000000010000
#define SYN_ACK 0b0000000000010010

typedef struct {                
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


unsigned int checksum(unsigned short int *arr);			// calculate checksum
unsigned short int flag_SYN( tcp_hdr * tcp_seg);		// SYN bit to 1
unsigned short int flag_SYN_ACK( tcp_hdr* tcp_seg);		// SYN and ACK bit to 1
unsigned short int flag_ACK( tcp_hdr* tcp_seg);			// ACK bit to 1
unsigned short int flag_FIN( tcp_hdr* tcp_seg);			// FIN bit to 1

uint32_t generate_inital_seq();
uint32_t generate_inital_seq2();

void print_struct( tcp_hdr * tcp_seg);				
char * print_binary_flags(tcp_hdr * tcp_seg);				// print binary format
void output(FILE * writeFile, tcp_hdr * tcp_seg);		// write to File

int main()
{

	// struct for packet header
	tcp_hdr tcp_seg;		// opening TCP connection
	tcp_hdr tcp_close;		// closing TCP connection
	unsigned int cksum;		// check sum value
	unsigned short int chksum_array[12];
	unsigned short int header_length;
	unsigned short int temp_client_seq;
	unsigned short int temp_server_seq;


	//create server address
	struct sockaddr_in servaddr;	
	int sock_fd, conn_fd, n;
	int write_fd;

	//write into a fiile
	FILE * writeFile;

//--------------------------------------------------------------------------------

	if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error\n");
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;    
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(1500); 		//port

	int on = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));		// make sure there is not binding error


	/* Binds the above details to the socket */
	
	if ( bind(sock_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0)
	{
		printf("socket bind failed...\n");
		exit(1);
	}
	else {	printf("Succesfully binded\n");}

	/* Listen to incoming connections*/
	if(listen(sock_fd, 10) != 0 )
	{
		printf("listen failed...\n");
		exit(1);
	}
	else {printf("server listening\n\n");}


	conn_fd = accept(sock_fd, (struct sockaddr*) NULL, NULL);	// establish connection

	writeFile = fopen("server.out", "w+");	// write into file
	fprintf(writeFile,"%s\n", "--- Server Received TCP segment ---");
//--------------------------------------------------------------------------------
// receive TCP request from client
	printf("----Received SYN----\n");
	printf("Opening TCP connection...\n");
	
	fprintf(writeFile, "%s\n", "Opening TCP connection...");

	header_length = sizeof (tcp_seg);	// struct length
	header_length = header_length << 11;	// shift 11 bits
	

	n = read(conn_fd, &tcp_seg, sizeof(tcp_seg));		// receive SYN
	output(writeFile, &tcp_seg);		// write to server.out

	print_struct(&tcp_seg);	// print out the struct of tcp segment


	temp_client_seq = tcp_seg.seq;			// temporarily save the current client sequence number

//--------------------------------------------------------------------------------
	tcp_seg.ack = tcp_seg.seq + 1;		// ack # equal to initial client sequence number + 1
	tcp_seg.seq = generate_inital_seq();		// initial server sequence number 

	temp_server_seq = tcp_seg.seq;		// temporarily save current server sequence number

	tcp_seg.hdr_flags = header_length;		// assign header legnth
	tcp_seg.hdr_flags = flag_SYN_ACK(&tcp_seg); // set the SYN bit and ACK bit to 1 
	

	tcp_seg.cksum = 0;		// reset cksum to 0
	
	memcpy(chksum_array, &tcp_seg, 24);	// copy memory address to array
	cksum = checksum(chksum_array);		// compute checksum
	tcp_seg.cksum = cksum;		// assign checksum to tcp segment

	
	write_fd = write(conn_fd, (tcp_hdr*) & tcp_seg , sizeof(tcp_seg)); // send SYN and ACK

	


//--------------------------------------------------------------------------------
	printf("----Received ACK----\n");
	n = read(conn_fd, &tcp_seg, sizeof(tcp_seg));		// receive ACK
	output(writeFile, &tcp_seg);		// write to server.out
	//printf("seq:%d\n", tcp_seg.seq);
	print_struct(&tcp_seg);
	printf("Connection established! Client is live!\n\n");



//--------------------------------------------------------------------------------
// Receiving data
	int i = 1;
	int k = 0;

	printf("----Receiving data----\n\n");

	tcp_seg.hdr_flags = header_length;		// assign header legnth
	tcp_seg.hdr_flags = flag_ACK(&tcp_seg); // set the ACK bit to 1 
	
	while(k < 8)
{


	n = read(conn_fd, &tcp_seg, sizeof(tcp_seg));		// read data
	printf("Segment %d: FILE RECEIVE\n",i);
	print_struct(&tcp_seg);


	tcp_seg.payload[128] ='\0';
	//printf("%s", tcp_seg.payload);		// print out the data

	fprintf(writeFile, "%s%d%s\n", "Segment ", i, ": FILE RECEIVE");
	output(writeFile, &tcp_seg);		// write to client.out

	temp_server_seq = tcp_seg.ack;		// keep track of current client sequence
	temp_client_seq = tcp_seg.seq;		// keep track of current server sequence


	tcp_seg.seq = temp_server_seq + 1;		// increment 128 everytime receive 128 bytes	
	tcp_seg.ack =  temp_client_seq + 128;		// increment server sequence number by 2

	

	tcp_seg.cksum = 0;		// reset cksum to 0
	
	memcpy(chksum_array, &tcp_seg, 24);	// copy memory address to array
	cksum = checksum(chksum_array);		// compute checksum
	tcp_seg.cksum = cksum;		// assign checksum to tcp segment

	write_fd = write(conn_fd,  (tcp_hdr*) &tcp_seg , sizeof(tcp_seg) ); // send ACK

	k++;
	i++;
}


	printf("\n");









//--------------------------------------------------------------------------------
// receive closing connection signal from client
	printf("----Received FIN----\n");
	printf("Preparing to close TCP connection...\n");
	fprintf(writeFile, "%s\n", "Preparing to close TCP connection...");
	n = read(conn_fd, &tcp_close, sizeof(tcp_close));	// receive FIN
	
	output(writeFile, &tcp_close);		// write to server.out
	print_struct(&tcp_close);


//--------------------------------------------------------------------------------
// The server responds back with an acknowledgment TCP segment
	tcp_close.ack = tcp_close.seq +1;	//  ack number equal to initial client sequence number + 1 
	tcp_close.seq = generate_inital_seq2();		// intial server sequence number
	
	tcp_close.hdr_flags = header_length;		// assign header legnth
	tcp_close.hdr_flags = flag_ACK(&tcp_close);		// set the ACK bit to 1 

	tcp_close.cksum = 0;		// reset cksum to 0
	
	memset(chksum_array, 0 , sizeof(chksum_array));		// clear array
	memcpy(chksum_array, &tcp_close, 24);	// copy memory address to array
	cksum = checksum(chksum_array);		// compute checksum
	tcp_close.cksum = cksum;		// assign checksum to tcp segment


	write_fd = write(conn_fd, (tcp_hdr*) & tcp_close , sizeof(tcp_close) ); // send ACK #

	
//--------------------------------------------------------------------------------
// The server again sends another close acknowledgement TCP segment


	tcp_close.hdr_flags = header_length;		// assign header legnth
	tcp_close.hdr_flags = flag_FIN(&tcp_close);		// set the FIN bit to 1 


	tcp_close.cksum = 0;		// reset cksum to 0
	
	memset(chksum_array, 0 , sizeof(chksum_array));		// clear array
	memcpy(chksum_array, &tcp_close, 24);	// copy memory address to array
	cksum = checksum(chksum_array);		// compute checksum
	tcp_close.cksum = cksum;		// assign checksum to tcp segment


	write_fd = write(conn_fd, (tcp_hdr*) & tcp_close , sizeof(tcp_close) ); // send FIN


//--------------------------------------------------------------------------------
// The server receive TCP ack # indicates connection closed
	printf("----Receive ACK----\n");
	n = read(conn_fd, &tcp_close ,sizeof(tcp_close));		// receive ACK
	output(writeFile, &tcp_close);		// write to server.out

	print_struct(&tcp_close);


	fclose(writeFile);
	close(conn_fd);		// close connection
	close (sock_fd);

		

	return 0;
}



unsigned int checksum(unsigned short int *arr)		// Compute sum   
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
	return rand() % 1000 + 100;
}

uint32_t generate_inital_seq2()
{
	return rand() % 100000 + 100;
}

unsigned short int flag_SYN( tcp_hdr* tcp_seg)
{
	tcp_seg->hdr_flags = tcp_seg->hdr_flags | SYN;		// assing 1 bit

	return tcp_seg->hdr_flags;
}
unsigned short int flag_SYN_ACK( tcp_hdr* tcp_seg)
{
	tcp_seg->hdr_flags = tcp_seg->hdr_flags | SYN_ACK;		// assing 1 bit

	return tcp_seg->hdr_flags;
}


unsigned short int flag_ACK( tcp_hdr* tcp_seg)
{
	tcp_seg->hdr_flags = tcp_seg->hdr_flags | ACK;		// assing 1 bit
	
	return tcp_seg->hdr_flags;
}

unsigned short int flag_FIN( tcp_hdr* tcp_seg)
{
	tcp_seg->hdr_flags = tcp_seg->hdr_flags | FIN;		// assing 1 bit
	
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