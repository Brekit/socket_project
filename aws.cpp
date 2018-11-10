
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

#define servAPort 21687
#define servBPort 22687
#define servCPort 23687
#define UDPport 24687
#define clientTCP 25687
#define MonitorTCP 26687


int recieveClient(int socket, struct sockaddr_in server, int socketa){
  int Values[3];
  recv(socket,Values, 3*sizeof(int),0);
  printf("Link:%d\nSize:%d\nPower:%d\n", Values[0], Values[1], Values[2]);

  if (sendto(socketa, (char*)Values, 3*sizeof(int), 0, (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    perror("Send to server A failed");
    return -1;
  }

  //return *Values;
}

/*
int sendToAllThree(int serva, int servb, int servc, int *Vals, struct servAInfo){
  if (sendto(serva, (char*)Vals, 3*sizeof(int), 0, (struct sockaddr *)&servAInfo, sizeof(servAInfo)) < 0){
    perror("failed to send to server A\n");
    return -1;
  } else {
    printf("AWS sent link=%d, size=%d, power=%d to server A.\n", Vals[0],Vals[1],Vals[2]);
  }

  if (send(servb, (char*)Vals, 3*sizeof(int), 0) < 0){
    perror("failed to send to server A\n");
    return -1;
  } else {
    printf("AWS sent link=%d, size=%d, power=%d to server B.\n", Vals[0],Vals[1],Vals[2]);
  }

  if (send(servc, (char*)Vals, 3*sizeof(int), 0) < 0){
    perror("failed to send to server A\n");
    return -1;
  } else {
    printf("AWS sent link=%d, size=%d, power=%d to server C.\n", Vals[0],Vals[1],Vals[2]);
  }
}
*/

int main(){
  printf("The AWS is up and running\n");


  int cli_soc, a_soc, b_soc, c_soc, mon_soc , newsock;
  struct sockaddr_in client, serverA, serverB, serverC, monitor;
  int addrlen = sizeof(client);
  //char buffer[2048] = {0};
  int target=0;
  int Vals[3];
  int valread;
  memset(&serverA,'0',sizeof(serverA));
  // ============ Create All the Sockets ============ //

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
  serverA.sin_port = htons(servAPort);

  if((b_soc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server B socket creation failed");
    return -1;
  }
  serverB.sin_family = AF_INET;
  serverB.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverB.sin_port = htons(servBPort);

  if((c_soc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server C socket creation failed");
    return -1;
  }
  serverC.sin_family = AF_INET;
  serverC.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverC.sin_port = htons(servCPort);

  if((mon_soc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Monitor socket cretion failed");
    return -1;
  }
  monitor.sin_family = AF_INET;
  monitor.sin_addr.s_addr = inet_addr("127.0.0.1");
  monitor.sin_port = htons(clientTCP);

  // ============ Let's bind ============ //
  if (bind(cli_soc,  (struct sockaddr *)&client, sizeof client) < 0)
  {
    perror("\nbind to socket failed");
    return -1;
  }

  if (bind(b_soc,  (struct sockaddr *)&serverB, sizeof serverB) < 0)
  {
    perror("\nbind to server B failed");
    return -1;
  }

  if (bind(c_soc,  (struct sockaddr *)&serverC, sizeof serverC) < 0)
  {
    perror("\nbind to server C failed");
    return -1;
  }

  if (bind(mon_soc,  (struct sockaddr *)&monitor, sizeof monitor) < 0)
  {
    perror("\nbind to monitor failed");
    return -1;
  }

  // ============ Listen from client and send to server A,B,C ============ //

  if (listen(cli_soc,6) < 0)
  {
    perror("\nlisten failed");
    return -1;
  }

  int new_socket = accept(cli_soc, (struct sockaddr *)&client,(socklen_t*)&addrlen);

  int x = recieveClient(new_socket, serverA, a_soc);


//sendToAllThree(a_soc, b_soc, c_soc, &x, struct serverA);




}
