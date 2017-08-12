#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <sstream>
void send_to_serverOr(int sock_udp_send, sockaddr_in server_or_receive, socklen_t backend_length,
		      int sock_udp_receive, sockaddr_in server_or_send, char* buffer);

void send_to_serverAnd(int sock_udp_send, sockaddr_in server_and_receive, socklen_t backend_length,
		       int sock_udp_receive, sockaddr_in server_and_send, char* buffer);

void getResult(int sock_udp_receive, sockaddr_in backend, socklen_t backend_length, char* buffer);
