#include "edge.h"
using namespace std;
/*
  The functions getaddrinfo(), socket(), bind(), listen(), accept, send(), recv(), sendto(), recvfrom() are referenced from Beej's socket programming.

 */
int main(int argc, char* argv[]){
  const char* port_tcp = "23476"; // the static tcp port on server edge
  unsigned short int port_or = 21476; // the static udp port on server or
  unsigned short int port_edge_udp = 24476; // the static udp port on server edge
  unsigned short int port_and = 22476; // the static udp port on server and
  
  addrinfo hints_tcp, *resolved; // make hints for resolving the server on tcp port, and create "resolved" to hold the result
  memset(&hints_tcp, 0, sizeof(hints_tcp));
  hints_tcp.ai_family = AF_INET; // IPv4
  hints_tcp.ai_socktype = SOCK_STREAM; // data stream : tcp
  hints_tcp.ai_flags = AI_PASSIVE; // fill in the local ip

  if(getaddrinfo(NULL, port_tcp, &hints_tcp, &resolved) != 0){
    cout << "Server: Cannot resolve the address.\n";
    return 1;
  }

  int sock_descriptor; // the sock descriptor, this is the listening parent socket that live long in the edge server
  if((sock_descriptor = socket(resolved->ai_family, resolved->ai_socktype, resolved->ai_protocol)) == -1){
    cout << "Server: Cannot get the socket descriptor.\n";
    return 1;
  }
  // bind the socket to the tcp port address
  if(bind(sock_descriptor, resolved->ai_addr, resolved->ai_addrlen) == -1){
    cout << "Server: Cannot bind.\n";
    close(sock_descriptor);
    return 1;
  }
  // now listen to any incoming clients
  if(listen(sock_descriptor, 5) == -1){
    cout << "Server: Cannot listen.\n";
    close(sock_descriptor);
    return 1;
  }


  //------------------------------- Edge - Server_or ----------------------------//
  int sock_udp_send, sock_udp_receive; // edge server has two udp sockets, one for sending data and one for receiving
  sockaddr_in server_or_receive, server_or_send, edge_udp; // server or also has two udp sockets
  memset(&server_or_receive, 0, sizeof(server_or_receive));// always clear
  // only need to initialize the server or receive socket, because we need to send data to this socket. When receiving data from server or, we can have empty send_socket to capture it.
  server_or_receive.sin_family = AF_INET;
  server_or_receive.sin_addr.s_addr = INADDR_ANY; // accept data of any incoming datagram
  server_or_receive.sin_port = port_or;

  // this is the server edge udp receiving socket address
  memset(&edge_udp, 0, sizeof(edge_udp));
  edge_udp.sin_family = AF_INET;
  edge_udp.sin_addr.s_addr = INADDR_ANY;
  edge_udp.sin_port = port_edge_udp;

  // create the socket of udp for sending data
  if((sock_udp_send = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    cout << "Server: Cannot get the socket descriptor of UDP sending.\n";
    close(sock_descriptor);
    return 1;
  }
  // create the socket of udp for receiving data
  if((sock_udp_receive = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    cout << "Server: Cannot get the socket descriptor of UDP receiving.\n";
    close(sock_descriptor);
    close(sock_udp_send);
    return 1;
  }
  // bind the address with port number to the receiving socket
  if(bind(sock_udp_receive, (sockaddr*) &edge_udp, sizeof(edge_udp)) == -1){
    cout << "Server: Cannot bind.\n";
    close(sock_descriptor);
    close(sock_udp_send);
    close(sock_udp_receive);
    return 1;
  }

  //------------------------------- Edge - Server And ---------------------------//
  sockaddr_in server_and_receive, server_and_send;
  memset(&server_and_receive, 0, sizeof(server_and_receive));
  server_and_receive.sin_family = AF_INET;
  server_and_receive.sin_addr.s_addr = INADDR_ANY;
  server_and_receive.sin_port = port_and; //this is the port number on and server side


  socklen_t backend_length = sizeof(struct sockaddr_in);
  //------------------------------- Edge - Client transfer ---------------------------//
  cout << "The edge server is up and running." << endl;
  while(1){
    sockaddr client_addr; // capture the incoming client address
    socklen_t size_addr = sizeof(client_addr);
    int child_sock = accept(sock_descriptor, &client_addr, &size_addr); // create a child socket by accepting the incoming client, from now on, the client communicate with server only on this socket
    if(child_sock == -1){
      cout << "Server: Child socket created failed.\n";
    }
    int receive = -1; // this is the length of bytes received from the client, initialized to -1, when it becomes -1 again, means that the client has gotten all wanted result and closed the connection
    int count = 0;
    int count_or = 0;
    int count_and = 0;
    string delimiter = ",";
    string op, operand1_str, operand2_str;
    cout << "The edge server start receiving the computation results from Backend-Server OR and Backend-Server AND using UDP over port " << port_edge_udp << ".\n";
    cout << "The computation results are:" << endl;
    while(receive != 0 && child_sock != -1){
      // while stops when the client closes the connection
      char buffer[255]; // capture the incoming data
      int length_buffer = 255;
      memset(buffer, 0, length_buffer);
      if((receive = recv(child_sock, buffer, length_buffer, 0)) == -1){
	cout << "Server: Cannot receive message from client.\n";
      }
      count++;

      string line = buffer;
      int pos = line.find(delimiter);
      op = line.substr(0, pos);
      int pos2 = line.find(delimiter, pos + delimiter.length());
      operand1_str = line.substr(pos + delimiter.length(), pos2 - pos - 1);
      operand2_str = line.substr(pos2 + delimiter.length());

      strcpy(buffer, line.substr(pos + delimiter.length()).c_str()); // simply make sure the buffer has '\0'
      char result[255];
      memset(result, 0, 255);
      if(!op.compare("or")){
	// send operands to server or
	send_to_serverOr(sock_udp_send, server_or_receive, backend_length,
			 sock_udp_receive, server_or_send, buffer);
	count_or++;
	// get the result from server or
	getResult(sock_udp_receive, server_or_send, backend_length, result);
	cout << operand1_str << " or " << operand2_str << " = " << result << endl;
      }
      else if(!op.compare("and")){
	// send operands to server and
	send_to_serverAnd(sock_udp_send, server_and_receive, backend_length,
			  sock_udp_receive, server_and_send, buffer);
	count_and++;
	// get the result from server and
	getResult(sock_udp_receive, server_and_send, backend_length, result);
	cout << operand1_str << " and " << operand2_str << " = " << result << endl;
      }
      send(child_sock, result, strlen(result), 0); // send the result to the client
    }
    cout << "The edge server has received " << --count << " lines from the client using TCP over port " << port_tcp << ".\n";
    cout << "The edge has successfully sent " << count_or << " lines to Backend-Server OR.\n";
    cout << "The edge has successfully sent " << count_and << " lines to Backend-Server AND.\n";
    cout << "The edge server has successfully finished receiving all computation results from Backend-Server OR and Backend-Server AND.\n";
    cout << "The edge server has successfully finished sending all computation results to the client.\n";
    close(child_sock); // close the socket for client
  }
  close(sock_descriptor); // close the listening socket
  close(sock_udp_send); // close the udp send socket
  close(sock_udp_receive); // close the udp receive socket
  return 0;
}

void send_to_serverOr(int sock_udp_send, sockaddr_in server_or_receive, socklen_t backend_length,
		       int sock_udp_receive, sockaddr_in server_or_send, char* buffer){
  if(sendto(sock_udp_send, buffer, strlen(buffer), 0, (sockaddr*)&server_or_receive, backend_length) == -1){
    cout << "Edge: Cannot send UDP data to OR.\n";
  }
}


void send_to_serverAnd(int sock_udp_send, sockaddr_in server_and_receive, socklen_t backend_length,
		       int sock_udp_receive, sockaddr_in server_and_send, char* buffer){
  if(sendto(sock_udp_send, buffer, strlen(buffer), 0, (sockaddr*)&server_and_receive, backend_length) == -1){
    cout << "Edge: Cannot send UDP data to AND.\n";
  }
}

void getResult(int sock_udp_receive, sockaddr_in backend, socklen_t backend_length, char* buffer){
  if(recvfrom(sock_udp_receive, buffer, 255, 0, (sockaddr*)&backend, &backend_length) == -1){
    cout << "OR: Cannot receive result from backend server.\n";
  }
}
