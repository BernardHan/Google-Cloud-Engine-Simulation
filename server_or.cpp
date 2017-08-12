#include "backend_server.h"
using namespace std;
/*
  The sendto() and recvfrom() are referenced from the Beej's socket programming.
 */

int main(int argc, char* argv[]){
  const char* port_udp = "21476";
  unsigned short int port = 21476;
  unsigned short int port_edge = 24476;
  cout << "The Server OR is up and running using udp on port " << port_udp << endl;
  cout << "The Server OR start receiving lines from the edge server for OR computation. The compulation results are:" << endl;

  int sock_receive;
  sockaddr_in server_or;
  memset(&server_or, 0, sizeof(server_or));
  server_or.sin_family = AF_INET; // IPv4
  server_or.sin_addr.s_addr = INADDR_ANY; // receive data from any datagram
  server_or.sin_port = port;

  // create the socket for receive port
  if((sock_receive = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    cout << "OR: Cannot get the socket descriptor of OR.\n";
    return 1;
  }
  // bind the receiving socket to the static udp port
  if(bind(sock_receive, (sockaddr*) &server_or, sizeof(server_or)) == -1){
    cout << "OR: Cannot bind.\n";
    close(sock_receive);
    return 1;
  }

  int sock_send;
  // create the sending socket, but not bind it, because we don't need a associated port to send
  if((sock_send = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    cout << "OR: Cannot get the socket send of OR.\n";
    return 1;
  }

  // establish the socket address of the server edge on udp port
  sockaddr_in edge_receive;
  memset(&edge_receive, 0, sizeof(edge_receive));
  edge_receive.sin_family = AF_INET;
  edge_receive.sin_addr.s_addr = INADDR_ANY;
  edge_receive.sin_port = port_edge;


  sockaddr_in edge_send;
  socklen_t edge_send_length = sizeof(struct sockaddr_in);
  string delimiter = ",";
  string operand1_str, operand2_str;
  int count = 0;
  while(1){
    char buffer[255];
    memset(buffer, 0, 255);
    int receive = -1;
    // receive operands from the edge server, capture the edge server udp socket address
    if((receive = recvfrom(sock_receive, buffer, 255, 0, (sockaddr*)&edge_send, &edge_send_length)) == -1){
      cout << "OR: Cannot receive from edge.\n";
    }

    string line = buffer;
    int position1 = line.find(delimiter);
    operand1_str = line.substr(0, position1);
    operand2_str = line.substr(position1 + delimiter.length());

    //int operand1 = stoi(operand1_str, NULL, 2); //stoi is introduced in c++11
    //int operand2 = stoi(operand2_str, NULL, 2);

    long int operand1, operand2;
    // convert the binary string to long int
    operand1 = strtol(operand1_str.c_str(), NULL, 2); 
    operand2 = strtol(operand2_str.c_str(), NULL, 2);

    int result = operand1 | operand2;
    bitset<32> result_bits(result); // build a 32 bits bitset out of the result
    string result_str = result_bits.to_string(); // convert it into string

    int first_one;
    // trim all the leading 0s
    if((first_one = result_str.find('1')) != string::npos){
      result_str = result_str.substr(first_one);
    }
    else{
      // the result is 0
      result_str = result_str.substr(result_str.length() - 1);
    }
    cout << operand1_str << " or " << operand2_str << " = " << result_str << endl;
    char msg[result_str.length() + 1];
    memset(msg, 0, result_str.length() + 1);
    strcpy(msg, result_str.c_str());
    // send the result back to the edge server
    if(sendto(sock_send, msg, strlen(msg), 0, (sockaddr*)&edge_receive, sizeof(struct sockaddr_in)) == -1){
      cout << "OR: cannot send result to edge\n";
    }
    count++;
    cout << "The Server OR has successfully received " << count << " lines from the edge server and finished all AND computations\n";
    cout << "The Server OR has successfully finished sending all computation results to the edge server.\n";
  }
  // need to make loop to receive, also try getaddrinfo later instead of filling mannually sockaddr

  return 0;
}
