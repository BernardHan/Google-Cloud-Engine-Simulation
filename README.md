#### Introduction
This is a simple simulation of google cloud engine, which include an edge server, two backend servers and a client. The client connects to the edge server via TCP, and sends computational requests. Once the edge server receives the request through TCP, it parses and figures which backend server the request should be sent to. Then the edge server sends it to the backend server through UDP. When the backend servers receive the requests via UDP, they compute and send the results back to the edge server through UDP again. Once the edge server receives the result, it sends it back to the client through TCP again.

#### File Structure
client.cpp & client.h: The client create socket and connect to server. Then it read the file indicated by the user input, and pass each line to the server line by line. It waits for the result of each line and then pass the next line.

edge.cpp & edge.h: The server create a TCP listening socket for incoming client. In a while(1) loop, it accepts incoming client, and then receives each line from the client until the client close connection. Once it receives a line, it parse it into just number string and pass to corresponding backend server through UDP socket just for sending data. Then it waits for the result through a UDP socket just for receiving data. After receiving the results from the backend server, it sends the results back to the client through TCP.

server_or.cpp & backend_server.h: This server receives requests from edge server line by line, and "or" the two operands together, and then sends the result back to the edge server through a UDP socket just for sending.

server_and.cpp & backend_server.h: same as server_or except that it only does "and" operation.


#### How to Use
Open 4 terminals, in one terminal type "make all". Then type "make server_or", "make server_and", "make edge" on each of the 3 terminals. On the last terminal, use "./client [job text file]" to do the magic.

To stop servers, use Ctrl-C.

#### Example Output:
client >> edge: original line from the input file
edge >> backend: "[operand1],[operand2]", ex: "101,1001"
backend >> edge: "[result]"
edge >> client: "[result]"

The server_and and server_or receive and send back line by line instead of receiving and sending back multiple lines. So the on-screen message about how many lines computed and the statement "finished sending all results" will be printed each time after they send one result.

#### Reference: 
Beej tutorial: http://beej.us/guide/bgnet/output/html/multipage/syscalls.html