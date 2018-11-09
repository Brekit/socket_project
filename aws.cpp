
//1 UDP, 24000+xxx1 TCP with client, 25000+xxx1 TCP with monitor, 26000+xxx
//UDP 24687
// TCP with Client 25687
//TCP with Monitor 26687
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

#define UDPport 24687
#define clientTCP 25687
#define MonitorTCP 26687


int recieveClient(int socket){
  int Vals[3];
  recv(socket,Vals, 3*sizeof(int),0);
  printf("Link:%d\nSize:%d\nPower:%d\n", Vals[0], Vals[1], Vals[2]);
  return *Vals;
}


int main(){
  printf("The AWS is up and running\n");


  int sock_fd, newsock;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  //char buffer[2048] = {0};
  int target=0;
  int Vals[3];
  int valread;

  if((sock_fd = socket(AF_INET, SOCK_STREAM,0)) == 0)
  {
    printf("\nerror, Socket cretion failed");
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(clientTCP);

  if (bind(sock_fd,  (struct sockaddr *)&address, sizeof address) < 0)
  {
    perror("\nbind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(sock_fd,6) < 0)
  {
    perror("\nlisten failed");
    exit(EXIT_FAILURE);
  }

  int new_socket = accept(sock_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen);
  recieveClient(new_socket);
}
