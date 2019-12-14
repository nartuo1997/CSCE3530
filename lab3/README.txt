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

 


