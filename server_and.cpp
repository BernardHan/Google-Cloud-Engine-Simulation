#include "backend_server.h"
using namespace std;
/*
  The sendto() and recvfrom() are referenced from the Beej's socket programming.
 */

int main(int argc, char* argv[]){
  const char* port_udp = "22476";
  unsigned short int port = 22476;
  unsigned short int port_edge = 24476;
  cout << "The Server AND is up and running using udp on port " << port_udp << endl;
  cout << "The Server AND start receiving lines from the edge server for AND computation. The compulation results are:" << endl;

  int sock_receive;
  sockaddr_in server_and;
  memset(&server_and, 0, sizeof(server_and));
  server_and.sin_family = AF_INET;
  server_and.sin_addr.s_addr = INADDR_ANY;
  server_and.sin_port = port;
  
  if((sock_receive = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    cout << "AND: Cannot get the socket descriptor of AND.\n";
    return 1;
  }

  if(bind(sock_receive, (sockaddr*) &server_and, sizeof(server_and)) == -1){
    cout << "AND: Cannot bind.\n";
    close(sock_receive);
    return 1;
  }

  int sock_send;
  if((sock_send = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    cout << "AND: Cannot get the socket send of AND.\n";
    return 1;
  }

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
    if((receive = recvfrom(sock_receive, buffer, 255, 0, (sockaddr*)&edge_send, &edge_send_length)) == -1){
      cout << "AND: Cannot receive from edge.\n";
    }

    string line = buffer;
    int position1 = line.find(delimiter);
    operand1_str = line.substr(0, position1);
    operand2_str = line.substr(position1 + delimiter.length());
    // stoi is introduced in c++11
    //int operand1 = stoi(operand1_str, nullptr, 2);
    //int operand2 = stoi(operand2_str, nullptr, 2);
    long int operand1, operand2;
    operand1 = strtol(operand1_str.c_str(), NULL, 2);
    operand2 = strtol(operand2_str.c_str(), NULL, 2);

    int result = operand1 & operand2;

    bitset<32> result_bits(result);
    string result_str = result_bits.to_string();

    int first_one;
    if((first_one = result_str.find('1')) != string::npos){
      result_str = result_str.substr(first_one);
    }
    else{
      // the result is 0
      result_str = result_str.substr(result_str.length() - 1);
    }
    cout << operand1_str << " and " << operand2_str << " = " << result_str << endl;
    char msg[result_str.length() + 1];
    memset(msg, 0, result_str.length() + 1);
    strcpy(msg, result_str.c_str());
    if(sendto(sock_send, msg, strlen(msg), 0, (sockaddr*)&edge_receive, sizeof(struct sockaddr_in)) == -1){
      cout << "AND: cannot send result to edge\n";
    }
    count++;
    cout << "The Server AND has successfully received " << count << " lines from the edge server and finished all AND computations\n";
    cout << "The Server AND has successfully finished sending all computation results to the edge server.\n";
  }
  // need to make loop to receive, also try getaddrinfo later instead of filling mannually sockaddr

  return 0;
}

