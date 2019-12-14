// Khang Tran - ID:11308109


Code usage example:
Please follow the exact format as below in order to execute the server and client.
 -- server.c
gcc -o server server.c
./server

Example:
pserver <port> 
pserver: 1200

 -- client.c
gcc -o client client.c

Example:
client <port>
client: 1200

url <url>
url: www.cse.unt.edu/~qingyang/

								-------------	List of testing url ------------
It is best to type in exactly as listed for accurate result.

List of URL:						

www.cse.unt.edu/~qingyang/research.html

www.cse.unt.edu/~qingyang/teaching.html

www.cse.unt.edu/~qingyang/publications.html

www.cse.unt.edu/~qingyang/services.html

www.cse.unt.edu/~qingyang/links.html

www.cse.unt.edu/~blanco/

www.cse.unt.edu/~qingyang/


List of blacklist:

www.unt.edu

www.cse.unt.edu/~blanco/

www.google.com

www.cse.unt.edu/~qingyang/

www.amazon.com

www.ebay.com


List of bad request URL:
www.bing.com		(this may take a minute to get back the reponse. Please be patient!)
www.jpf.go.jp
www.jnto.go.jp






---------------------------------------------------------------------------------------------

The server.c and client.c will use sockaddr_in family to connect each other. 

The protocol for this connection will be TCP

** server.c **
The server will get the input from user to execute the proxy server and select desired port number
to connect client. 

Server will run in an infinite loop to keep receiving and forwarding the url and html webpage.

It will parse the input string into 2 parts, first is pserver for proxy server executable, second is
port number that will be used in sin_port to connect client.

If user input in the wrong format, the program will exit.

Then the program will listen and bind to prepare for client incoming connection.

After succesful accepting connection with client, it will read the url request from client.

1/ If the url request is blocked within the blacklist time		(Block URL found!)

It will forward back a response indicates that the url has been blocked

To do this, simply read in the blacklist.txt, check whether the request url match with blocked url

If matched, then check the whether the request time is within the blocking period or not.

If the request time within the blocking period, send back "Website Blocked", else continue .


2/ If the url has already been cached		(Cache file founded! Sending cache file to client...)

The program will read the list of urls before deciding whether to cache the sending request.

If the url has already been cached, it will be present in the list.txt

To do this, simplyread in the list.txt, check whether the request url match with the cached url.

If matched, then read in the time request as it is also the name of the cache file.

Then simply copy the whole cache page into a buffer and send it back to client.

There will be no need of caching a new webpage if this happened.


3/ If the url has not been cached

a/ If HTTP response are not 200

Simply forward the HTTP response back to client

b/ If HTTP response are 200	(Cached the webpage...)

The server will receive request URL

It will parse the url into 2 parts: first is anything before the first "/", seocnd is anything after the first "/"

Then there will be an if(strcmp(second_part, " ")==0) to make check if url has any extra character after "/"

After that, it will use sprintf to combine "GET /" request.

The program will use addrinfo family to connect to webserver via port 80. 

After successfully creating a new socket and connecting to the webserver, it will send the http request
to webserver and wait for the response with html webpage source.

After collecting the whole webpage, it will forward to client.

At the same time, it will store the requested URL into a list.txt along with the time visited.

The time visited will also be the name of the cache file of that URL.

The server will keep updating the most 5 recent URL in the list as well as the cache file of those URL.



** client.c **

The program it will take in user input and parse it just like in server to make sure user enter the 
correct format. 

Then it will convert the port string into integer so it can be used in sin family.

The program will create a socket and use sin family to connect to server IP address and server port

Then it will connect the server through the socket just created if there are no errors.

The program will prompt user input in the desired url. After parsing the input, client will forward the 
url to proxy server and wait for the response along with the html webpage source.

Then eventually, the client will print out either the html source on the screen or Website Blocked or HTTP response if the request is bad.





