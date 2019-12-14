// Khang Tran - ID:11308109


Code usage example:
Please follow the exact format as below in order to execute the server and client.
 -- server.c
gcc -o server server.c
./server

Example:
pserver <port> 
pserver 1200

 -- client.c
gcc -o client client.c

Example:
client <port>
client 1200

url <url>
url: www.cse.unt.edu/~qingyang/ 

---------------------------------------------------------------------------------------------

The server.c and client.c will use sockaddr_in family to connect each other. 

The protocol for this connection will be TCP

** server.c **
The server will get the input from user to execute the proxy server and select desired port number
to connect client. 

It will parse the input string into 2 parts, first is pserver for proxy server executable, second is
port number that will be used in sin_port to connect client.

If user input in the wrong format, the program will exit.

Then the program will listen and bind to prepare for client incoming connection.

After succesful accepting connection with client, it will read the url request from client.

It will parse the url into 2 parts: first is anything before the first "/", seocnd is anything after the first "/"

Then there will be an if(strcmp(second_part, " ")==0) to make check if url has any extra character after "/"

After that, it will use sprintf to combine "GET /" request.

The program will use addrinfo family to connect to webserver via port 80. 

After successfully creating a new socket and connecting to the webserver, it will send the http request
to webserver and wait for the response with html webpage source.

After collecting the whole webpage, it will forward to client.

** client.c **

The program it will take in user input and parse it just like in server to make sure user enter the 
correct format. (client <port>)

Then it will convert the port string into integer so it can be used in sin family.

The program will create a socket and use sin family to connect to server IP address and server port

Then it will connect the server through the socket just created if there are no errors.

The program will prompt user input in the desired url. After parsing the input, client will forward the 
url to proxy server and wait for the response along with the html webpage source.

Then eventually, the client will print out the html source on the screen.





