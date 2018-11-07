
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

#define UDPport 24687
#define clinetTCP 25867
#define MonitorTCP 26687

int main(){
  printf("The AWS server has booted up\n");


  int sock_fd, newsock, valread;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[2048] = {0};

  if((sock_fd = socket(AF_INET, SOCK_STREAM,0)) == 0)
  {
    printf("\n error, Socket cretion failed");
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(UDPport);

  int bind(int sock_fd, const struct sockaddr *addr,socklen_t addrlen);
  listen(sock_fd,6);

  int new_socket = accept(sock_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen);

valread = read(new_socket, buffer, 2048);
printf("%s\n",buffer);
const char *test = "yo holmes, wazz good";
send(new_socket,test, strlen(test), 0);
printf("bruv we sent something\n");
return 0;
}
