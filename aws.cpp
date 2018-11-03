
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
#include <sys/wait.>

#define UDPport 24687
#define clinetTCP 25867
#define MonitorTCP 26687


using namespace std;

int main(){
  cout << "The AWS server has booted up\n";


  int sock_fd, newsock, valread;
  int addrlen= sizeof(address);
  char buffer[2048] = {0};

  if((sock_fd = socket(AF_INET, SOCK_STREAM,0)) == 0)
  {
    print("\n error, Socket cretion failed");
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INET_ADDR("127.0.0.1");
  address.sin_port = htons(UDPport);

  int bind(int sock_fd, (struct sockaddr *)&address,addrlen)<0){
    cout >> "\n error, Binding error";
    return -1;
  }

  if (listen(sock_fd),6 < 0){
    printf("\n error, Listening error");
    return -1;
  }

  new_socket = accept(sock_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen)

valread = read(new_socket, buffer, 2048);
printf("%s\n",buffer);
char *test = "can you hear me now?";
send(new_socket,test, strlen(test), 0);
printf("bruv we sent something\n");
return 0;
}
