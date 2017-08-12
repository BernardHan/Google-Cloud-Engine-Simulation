#include "client.h"
using namespace std;
/*
  The functions getaddrinfo(), socket(), connect(), send(), recv() are referenced from Beej's socket programming.

 */
int main(int argc, char* argv[]){
  if(argc != 2){
    cout << "Usage: ./client [Input Filename]\n";
    return 1;
  }

  cout << "The client is up and running.\n";

  const char* port_tcp = "23476";
  addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  int info;
  if((info = getaddrinfo(NULL, port_tcp, &hints, &res)) != 0){
    cout << "Client: cannot resolve the server address.\n";
    cout << "getaddrinfo: " << gai_strerror(info) << endl;
    return 1;
  }

  int sock_descriptor;
  if((sock_descriptor = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
    cout << "Client: Cannot get the socket descriptor.\n";
    return 1;
  }

  if(connect(sock_descriptor, res->ai_addr, res->ai_addrlen) == -1){
    cout << "Client: cannot connect to the server.\n";
    close(sock_descriptor);
    return 1;
  }

  string filename = argv[1];
  ifstream input(filename.c_str()); // input is the file
  string line;
  int count = 0;
  cout << "The final computation result are:" << endl;
  while(getline(input, line)){
    char msg[line.length() + 1];
    memset(msg, 0, line.length()+1);
    strcpy(msg, line.c_str()); // make sure the msg char array ends with \0
    int size = 0;
    // send the computation request to the edge server
    if((size = send(sock_descriptor, msg, strlen(msg), 0)) == -1){
      cout << "Client: Cannot send request.\n";
      close(sock_descriptor);
      return 1;
    }
    char result[255];
    memset(result, 0, 255);
    // receive the result from the edge server
    recv(sock_descriptor, result, 255, 0);
    count++;
    cout << result << endl;
  }
  cout << "The client has successfully finished sending " << count << " lines to the edge server.\n";
  cout << "The client has successfully finished receiving all computation results from the edge server.\n";
  close(sock_descriptor);
  return 0;
}
