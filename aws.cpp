
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
#define clinetTCP 25867
#define MonitorTCP 26687

int main(){
  printf("The AWS is up and running\n");


  int sock_fd, newsock, valread;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  //char buffer[2048] = {0};
  uint32_t target;

  if((sock_fd = socket(AF_INET, SOCK_STREAM,0)) == 0)
  {
    printf("\n error, Socket cretion failed");
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(UDPport);

  if (bind(sock_fd,  (struct sockaddr *)&address, sizeof address) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(sock_fd,6) < 0)
  {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  int new_socket = accept(sock_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen);

valread = recv(new_socket, &target, sizeof(target),0);
std::cout << target;

//send(new_socket,test, strlen(test), 0);
printf("AWS: Sent ACK to client\n");
return 0;
}
