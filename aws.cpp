
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


  int cli_soc, a_soc, b_soc, c_soc, mon_soc , newsock;
  struct sockaddr_in client, serverA, serverB, serverC, monitor;
  int addrlen = sizeof(client);
  //char buffer[2048] = {0};
  int target=0;
  int Vals[3];
  int valread;

  // ============ Create All the Sockets ============

  if((cli_soc = socket(AF_INET, SOCK_STREAM,0)) == 0)
  {
    printf("\nerror, client Socket cretion failed");
    return -1;
  }

  client.sin_family = AF_INET;
  client.sin_addr.s_addr = inet_addr("127.0.0.1");
  client.sin_port = htons(clientTCP);

  if((a_soc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server A socket creation failed");
    return -1;
  }

  serverA.sin_family = AF_INET;
  serverA.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverA.sin_port = htons(clientTCP);

  if((b_soc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server B socket creation failed");
    return -1;
  }

  serverB.sin_family = AF_INET;
  serverB.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverB.sin_port = htons(clientTCP);

  if((c_soc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server C socket creation failed");
    return -1;
  }

  serverC.sin_family = AF_INET;
  serverC.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverC.sin_port = htons(clientTCP);

  if((mon_soc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Monitor socket cretion failed");
    return -1;
  }

  monitor.sin_family = AF_INET;
  monitor.sin_addr.s_addr = inet_addr("127.0.0.1");
  monitor.sin_port = htons(clientTCP);


  if (bind(cli_soc,  (struct sockaddr *)&client, sizeof client) < 0)
  {
    perror("\nbind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(cli_soc,6) < 0)
  {
    perror("\nlisten failed");
    exit(EXIT_FAILURE);
  }

  int new_socket = accept(cli_soc, (struct sockaddr *)&client,(socklen_t*)&addrlen);
  recieveClient(new_socket);
}
