
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

struct FusedArray{
  int clientInput[3]={0};
  double dbValues[5]={0};
};

#define servAPort 21687
#define servBPort 22687
#define servCPort 23687
#define UDPport 24687
#define clientTCP 25687
#define MonitorTCP 26687

int sendData(int socket, struct sockaddr_in server, int *Data){
  if (sendto(socket, (char*)Data, 3*sizeof(int), 0, (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    perror("Send to server A failed");
    return -1;
  }
  else {
    printf("AWS submitted <%d> to servers\n",Data[0]);
  }
}



int *recieveClient(int socket){
  static int Values[3];
  recv(socket,Values, 3*sizeof(int),0);
  printf("Link:%d Size:%d Power:%d\n", Values[0], Values[1], Values[2]);
  return Values;
}


int main(){
  printf("The AWS is up and running\n");
  FusedArray Sample;

  int cli_soc, a_soc, b_soc, c_soc, mon_soc , newsock;
  struct sockaddr_in client, serverA, serverB, serverC, monitor;
  int addrlen = sizeof(client);
  int addrlen2 = sizeof(serverA);

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
  if (bind(mon_soc,  (struct sockaddr *)&monitor, sizeof monitor) < 0)
  {
    perror("\nbind to monitor failed");
    return -1;
  }

  // ============ Listen from client and send to server A,B,C ============ //
  int *x;
  double linkAVals[8];
  double linkBVals[8];

while(true){

  if (listen(cli_soc,6) < 0)
  {
    perror("\nlisten failed");
    return -1;
  }

  int new_socket = accept(cli_soc, (struct sockaddr *)&client,(socklen_t*)&addrlen);



  x = recieveClient(new_socket);

  sendData(a_soc, serverA, x);
  sendData(b_soc, serverB, x);


if (recvfrom(a_soc,linkAVals, 5*sizeof(double),0,(struct sockaddr *)&serverA,(socklen_t*)&addrlen2 ) < 0)
  {
    perror("Couldnt recieve from server A");
    return -1;
  }
else
  {
    if(int(linkAVals[0])==0)
    {
      //printf("link A val: %0f\n", linkAVals[0]);
      printf("The AWS recieved <0> matches from Backend-server <A> using UDP port <21687>\n");
    }
    else
    {
      //printf("link A val: %0f\n", linkAVals[0]);
      printf("The AWS recieved <1> matches from Backend-server <A> using UDP port <21687>\n");
      printf("sending to C (a)\n");
      //sendData(c_soc, serverC, x);
      //sendData(c_soc, serverC, linkAVals);
      //sendData(c_soc, serverC, linkAVals);

    }
  }

  if (recvfrom(b_soc,linkBVals, 5*sizeof(double),0,(struct sockaddr *)&serverB,(socklen_t*)&addrlen2 ) < 0)
    {
      perror("Couldnt recieve from server A");
      return -1;
    }
  else
    {
      if(int(linkBVals[0])==0)
      {
        //printf("link B val: %0f\n", linkBVals[0]);
        printf("The AWS recieved <0> matches from Backnend-server <B> using UDP port <22687>\n");
      }
      else
      {
        //printf("link A val: %0f\n", linkBVals[0]);

        printf("The AWS recieved <1> matches from Backend-server < B > using UDP port <22687>\n");
        printf("sending to C (b)\n");
        for(int i =0; i < 4; i++){
          Sample.clientInput[i] = x[i];
        }
        for(int j=0; j < 5; j++){
          Sample.dbValues[j] = linkAVals[j];
        }

        if (sendto(c_soc, (char*)&Sample, sizeof(&Sample), 0, (struct sockaddr *)&serverC , sizeof(serverC)) < 0)
        {
          perror("Send to server A failed");
          return -1;
        }

      }
    }

    // if (((int(linkAVals[0])==0) && (int(linkBVals[0])==0))
    // {
    //   //send message to both client and monitor that nothing was found
    // }

}

}
