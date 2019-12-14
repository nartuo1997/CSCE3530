How to compile:

---sever.c

gcc -o server server.c

./server



---client.c

gcc -o client client.c

./client



Code logic:

Opening TCP connection:
The server and client will use TCP socket connecting through port 1500

First the client will send the TCP segment with SYN bit on to tell the server there is an upcoming connection

Server will receive the SYN bit, it will send back an ACK and SYN bit to indicate the package has received

After receiving TCP segment from the server, the client will send back ACK bit to indicate package has been received.


Transfer file: The logic of this part has been confirmed by professor Qing Yang

Once the client receives the ACK segment, the next segment of 128 bytes is sent to the server with an appropriate sequence and acknowledgement numbers. 
Repeat till the entire file is transferred to the server

​After the client receives the above segment, it will assign:
sequence number       = initial client sequence number + 1;
ack number            = initial server sequence number + 128;
Then the client will send these 2 new numbers along with the new 128 bytes of data and keeps repeating until it's done.


Once the server receives the segment, the server responds back with an ACK segment with appropriate sequence number and acknowledgement number of the next byte

​So after the client sends the first 128 bytes of data, the server will assign:
sequence number       = initial client sequence number + 128;
ack number            = initial server sequence number + 2;
As the sequence number on client is increased by 128 byte in each round of communication. This is the second round, so the sequence number is initial client sequence number + 128.

The ACK number indicates what sequence number (on the server side) the client is expecting to receive in the next round. Here we assume the ACK contains one-byte data.



Closing TCP connection:
The client will send the TCP segment with FIN bit to tell the server ther is an upcomming closing connection

After receiving, server will send an ACK bit to tell client package has been received

After sending ACK, server will send FIN bit to tell client server is ready to close

After the client will receive both of ACK and FIN, it will send back ACK to tell server the package has
ben received.

Then both server and client will close the connection.

Everytime server received TCP segment from client, it will print out that segment to the console as well
as write it into the server.out file 

Everytime client received TCP segment from server, it will print out that segment to the console as well
as write it into the client.out file

 


