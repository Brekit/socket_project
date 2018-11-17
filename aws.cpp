#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>


#define servAPort 21687
#define servBPort 22687
#define servCPort 23687
#define UDPport 24687
#define clientTCP 25687
#define MonitorTCP 26687

struct FusedArray{
  int clientInput[3];
  double dbValues[5];
};

struct MonitorDataset{
  int clientInput[3];
  double CalculatedValues[3];
};

struct CalculatedValuesFromC{
  double ChannelCap, signalW, noiseW, dProp, dTrans, E2E;
};


int SendForCompute(int *array1, double *array2, int socket, struct sockaddr_in server){
  struct FusedArray FusePacket;
  for(int i=0; i < 4; i++){
    FusePacket.clientInput[i] = array1[i];
  }
  for(int j=0; j < 5; j++){
    FusePacket.dbValues[j] = array2[j];
  }
  if (sendto(socket, (char*)&FusePacket, sizeof(FusePacket), 0, (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    perror("Send to server C failed");
    return -1;
  }
  else
  {
    std::cout << "The AWS sent link ID=<" <<FusePacket.clientInput[0] << ">, size=<" << FusePacket.clientInput[1]<<">, power=<" << FusePacket.clientInput[2]<<">, and link information to Backend-Server C using UDP port <24687>" << std::endl;
  }
}

struct CalculatedValuesFromC recieveComputed(int socket, struct sockaddr_in *server, int address_length){
  struct CalculatedValuesFromC Dataset;
  if (recvfrom(socket, (void *) &Dataset, sizeof(Dataset),0,(struct sockaddr *)&server,(socklen_t*)&address_length) < 0)
  {
    perror("Couldnt recieve from server A");
  }
  else
  {
    std::cout << "The AWS recieved outputs from Backend-server C using UDP port <24687>" << std::endl;
  }
  return Dataset;
}

double * recieveFromA(int socket, struct sockaddr_in *server, int address_length, char Val){
  static double linkAVals[5];
  if (recvfrom(socket,linkAVals, 5*sizeof(double),0,(struct sockaddr *)&server,(socklen_t*)&address_length) < 0)
  {
    perror("Couldnt recieve from server A");
  }
  else
  {
    if(int(linkAVals[0])==0)
    {
      std::cout << "LinkVal :" << linkAVals[0] << std::endl;
      std::cout << "The AWS recieved <0> matches from Backend-server <" << Val << "> using UDP port <24687>" << std::endl;
    }
    else
    {
      std::cout << "LinkVal :" << linkAVals[0] << std::endl;
      std::cout << "The AWS recieved <1> matches from Backend-server <" << Val << "> using UDP port <24687>" << std::endl;
    }
  }
  return linkAVals;
}

double * recieveFromB(int socket, struct sockaddr_in *server, int address_length, char Val){
  static double linkBVals[5];
  if (recvfrom(socket,linkBVals, 5*sizeof(double),0,(struct sockaddr *)&server,(socklen_t*)&address_length) < 0)
  {
    perror("Couldnt recieve from server A");

  }
  else
  {
    if(int(linkBVals[0])==0)
    {
      std::cout << "LinkVal :" << linkBVals[0] << std::endl;
      std::cout << "The AWS recieved <0> matches from Backend-server <" << Val << "> using UDP port <24687>" << std::endl;
    }
    else
    {
      std::cout << "LinkVal :" << linkBVals[0] << std::endl;
      std::cout << "The AWS recieved <1> matches from Backend-server <" << Val << "> using UDP port <24687>" << std::endl;
    }
  }
  return linkBVals;
}


int sendData(int socket, struct sockaddr_in server, int *Data){
  if (sendto(socket, (char*)Data, 3*sizeof(int), 0, (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    perror("Send to server A failed");
    return -1;
  }
  else {
    std::cout << "AWS submitted <" << Data[0] << "> to servers" <<std::endl;
    return 0;
  }
}


int *recieveClient(int socket){
  static int Values[3];
  recv(socket,Values, 3*sizeof(int),0);
  printf("Link:%d Size:%d Power:%d\n", Values[0], Values[1], Values[2]);
  return Values;
}

int SendToMonitor(int socket, int *array1, double delayTrans, double delayProp, double E2EDelay){
  struct MonitorDataset Data;
  for (int i=0; i < 3; i++){
    Data.clientInput[i] = array1[i];
  }
  Data.CalculatedValues[0] = delayTrans;
  Data.CalculatedValues[1] = delayProp;
  Data.CalculatedValues[2] = E2EDelay;

  if (send(socket, &Data, sizeof(Data),0) < 0)
  {
    perror("Couldnt send to Monitor!");
  }
  else if(Data.CalculatedValues[0] != 0)
  {
    std::cout << "The AWS sent to detailed results to the monitor using TCP over port <26687>" << std::endl;
    return 0;
  }
  return 0;

}

int SendToClient (int socket, double e2eDelay){
  if (e2eDelay!= 0)
  {
    if (send(socket, &e2eDelay, sizeof(e2eDelay),0) < 0)
    {
      perror("Couldnt send to Monitor!");
    }
    else
    {
      std::cout << "The AWS sent delay=<" << e2eDelay << ">ms to the client using TCP port <25687>" << std::endl;
      return 0;
    }
  }
  return 0;
}


int main(){
  printf("The AWS is up and running\n");
  CalculatedValuesFromC CalculatedDatasetA, CalculatedDatasetB ;
  int cli_soc, a_soc, b_soc, c_soc, mon_soc, awsAsClient;
  struct sockaddr_in client, serverA, serverB, serverC, monitor, clientAws;
  int cli_len = sizeof(client);
  int servA_len = sizeof(serverA);
  int servB_len = sizeof(serverB);
  int servC_len = sizeof(serverB);
  int mon_len = sizeof(monitor);

  // ============ Create All the Sockets ============ //

  if((cli_soc = socket(AF_INET, SOCK_STREAM,0)) == 0)
  {
    printf("\nerror, client Socket cretion failed");
    return -1;
  }
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = inet_addr("127.0.0.1");
  client.sin_port = htons(clientTCP);

  if((awsAsClient = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server A socket creation failed");
    return -1;
  }
  clientAws.sin_family = AF_INET;
  clientAws.sin_addr.s_addr = inet_addr("127.0.0.1");
  clientAws.sin_port = htons(UDPport);


  serverA.sin_family = AF_INET;
  serverA.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverA.sin_port = htons(servAPort);

  serverB.sin_family = AF_INET;
  serverB.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverB.sin_port = htons(servBPort);

  serverC.sin_family = AF_INET;
  serverC.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverC.sin_port = htons(servCPort);

  if((mon_soc = socket(AF_INET, SOCK_STREAM,0)) == 0)
  {
    printf("\nerror, Monitor socket cretion failed");
    return -1;
  }
  monitor.sin_family = AF_INET;
  monitor.sin_addr.s_addr = inet_addr("127.0.0.1");
  monitor.sin_port = htons(MonitorTCP);


  // ============ Let's bind ============ //

  if (bind(awsAsClient,  (struct sockaddr *)&clientAws, sizeof clientAws) < 0)
  {
    perror("\nbind to monitor failed");
    return -1;
  }

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
  while(true){
  if (listen(mon_soc,6) < 0)
  {
    perror("\nlisten failed monitor");
    return -1;
  }

  if (listen(cli_soc,6) < 0)
  {
    perror("\nlisten failed client");
    return -1;
  }


    int *RecievedInputsFromClient;
    double *ResultsFromA;
    double *ResultsFromB;
    double *CalculatedValues;


  int cli_child = accept(cli_soc, (struct sockaddr *)&client,(socklen_t*)& cli_len);
  int mon_child = accept(mon_soc, (struct sockaddr *)&monitor,(socklen_t*)& monitor);

  RecievedInputsFromClient = recieveClient(cli_child);


  sendData(awsAsClient, serverA, RecievedInputsFromClient);
  ResultsFromA = recieveFromA(awsAsClient, &serverA, servA_len, 'A');
  sendData(awsAsClient, serverB, RecievedInputsFromClient);
  ResultsFromB = recieveFromB(awsAsClient, &serverB, servB_len, 'B');

  if ((int(ResultsFromA[0])==0) && (int(ResultsFromA[0])==0))
  {
    SendToMonitor(mon_child, RecievedInputsFromClient , 0,0,0);
    SendToClient(cli_child, 0);

  } else if (int(ResultsFromA[0])!=0){
    SendForCompute(RecievedInputsFromClient, ResultsFromA, awsAsClient, serverC);
    struct CalculatedValuesFromC CalculatedDatasetA = recieveComputed(awsAsClient, &serverC, servC_len);
    SendToMonitor(mon_child, RecievedInputsFromClient,CalculatedDatasetA.dTrans,CalculatedDatasetA.dProp,CalculatedDatasetA.E2E);
    SendToClient(cli_child, CalculatedDatasetA.E2E);

  } else if (int(ResultsFromB[0])!=0){
      SendForCompute(RecievedInputsFromClient, ResultsFromB, awsAsClient, serverC);
      struct CalculatedValuesFromC CalculatedDatasetB = recieveComputed(awsAsClient, &serverC, servC_len);
      SendToMonitor(mon_child, RecievedInputsFromClient,CalculatedDatasetB.dTrans,CalculatedDatasetB.dProp,CalculatedDatasetB.E2E );
      SendToClient(cli_child, CalculatedDatasetB.E2E);
    } else {
    SendToMonitor(mon_child, RecievedInputsFromClient , 0,0,0);

  }
  close(mon_child);
  close(cli_child);
}

}
