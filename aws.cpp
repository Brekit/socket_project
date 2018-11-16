
//1 UDP, 24000+xxx1 TCP with client, 25000+xxx1 TCP with monitor, 26000+xxx
//UDP 24687
// TCP with Client 25687
//TCP with Monitor 26687
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

struct FusedArray{
  int clientInput[3];
  double dbValues[5];
};

#define servAPort 21687
#define servBPort 22687
#define servCPort 23687
#define UDPport 24687
#define clientTCP 25687
#define MonitorTCP 26687

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
  //static double ComputedValues[8];
  //double linkBVals[5];c
if (recvfrom(socket, (void *) &Dataset, sizeof(Dataset),0,(struct sockaddr *)&server,(socklen_t*)&address_length) < 0)
  {
    perror("Couldnt recieve from server A");
  }
else
  {
      std::cout << "The AWS recieved outputs from Backend-server C using UDP port <24687>" << std::endl;
  }
  //close(socket);
  return Dataset;
}

double * recieveFromA(int socket, struct sockaddr_in *server, int address_length, char Val){
  static double linkAVals[5];
  //double linkBVals[5];c
if (recvfrom(socket,linkAVals, 5*sizeof(double),0,(struct sockaddr *)&server,(socklen_t*)&address_length) < 0)
  {
    perror("Couldnt recieve from server A");
    //return -1.00;
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
  //close(socket);
  return linkAVals;
}

double * recieveFromB(int socket, struct sockaddr_in *server, int address_length, char Val){
  static double linkBVals[5];
  //double linkBVals[5];c
if (recvfrom(socket,linkBVals, 5*sizeof(double),0,(struct sockaddr *)&server,(socklen_t*)&address_length) < 0)
  {
    perror("Couldnt recieve from server A");
    //return -1.00;
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
  //close(socket);
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

void SendAToMonitor(int socket, int *array1, double delayTrans, double delayProp, double E2EDelay){
double FusedDataSet[6];
for (int i=0; i < 4; i++){
FusedDataSet[i] = array1[i];
}
FusedDataSet[4] = delayTrans;
FusedDataSet[5] = delayProp;
FusedDataSet[6] = E2EDelay;

if (send(socket, FusedDataSet, sizeof(FusedDataSet),0) < 0)
  {
    perror("Couldnt recieve from server A");
  }
else
  {
      std::cout << "The AWS sent to monitor" << std::endl;
  }
  //close(socket);

}

void SendBToMonitor(int socket, int *array1, double delayTrans, double delayProp, double E2EDelay){
double FusedDataSet[6];
for (int i=0; i < 4; i++){
FusedDataSet[i] = array1[i];
}
FusedDataSet[4] = delayTrans;
FusedDataSet[5] = delayProp;
FusedDataSet[6] = E2EDelay;

if (send(socket, FusedDataSet, sizeof(FusedDataSet),0) < 0)
  {
    perror("Couldnt recieve from server A");
  }
else
  {
      std::cout << "The AWS sent to monitor" << std::endl;
  }
  //close(socket);

}

void SendToClient (){

}


int main(){
  printf("The AWS is up and running\n");
  //FusedArray Sample;
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


  // if((a_soc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  // {
  //   printf("\nerror, Server A socket creation failed");
  //   return -1;
  // }
  serverA.sin_family = AF_INET;
  serverA.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverA.sin_port = htons(servAPort);

  // if((b_soc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  // {
  //   printf("\nerror, Server B socket creation failed");
  //   return -1;
  // }
  serverB.sin_family = AF_INET;
  serverB.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverB.sin_port = htons(servBPort);

  // if((c_soc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  // {
  //   printf("\nerror, Server C socket creation failed");
  //   return -1;
  // }
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
  if (bind(cli_soc,  (struct sockaddr *)&client, sizeof client) < 0)
  {
    perror("\nbind to socket failed");
    return -1;
  }

  if (bind(awsAsClient,  (struct sockaddr *)&clientAws, sizeof clientAws) < 0)
  {
    perror("\nbind to monitor failed");
    return -1;
  }



  // ============ Listen from client and send to server A,B,C ============ //




  if (listen(cli_soc,6) < 0)
  {
    perror("\nlisten failed client");
    return -1;
  }


  int *RecievedInputsFromClient;
  double *ResultsFromA;
  double *ResultsFromB;
  double *CalculatedValues;

  int new_socket = accept(cli_soc, (struct sockaddr *)&client,(socklen_t*)& cli_len);

  RecievedInputsFromClient = recieveClient(new_socket);

  if (connect(mon_soc, (struct sockaddr *)&monitor, sizeof(monitor)) < 0)
  {
    perror("Connection Failed");
    return -1;
  }


  sendData(awsAsClient, serverA, RecievedInputsFromClient);
  ResultsFromA = recieveFromA(awsAsClient, &serverA, servA_len, 'A');
  sendData(awsAsClient, serverB, RecievedInputsFromClient);
  ResultsFromB = recieveFromB(awsAsClient, &serverB, servB_len, 'B');


  // std::cout << "----" << std::endl;
  //
  // std::cout << ResultsFromA[0] << std::endl;
  // std::cout << ResultsFromB[0] << std::endl;
  // std::cout << "----" << std::endl;

  if (int(ResultsFromA[0])!=0){
    SendForCompute(RecievedInputsFromClient, ResultsFromA, awsAsClient, serverC);
    struct CalculatedValuesFromC CalculatedDatasetA = recieveComputed(awsAsClient, &serverC, servC_len);
    //std::cout << "The AWSA sent delay=<" << std::setprecision(2) << CalculatedDatasetA.E2E << ">ms to the client using TCP over port <25687>" << std::endl;
    //SendToClient
    //std::cout << "The AWSA sent delay=<" << std::setprecision(2) << CalculatedDatasetA.dProp << ">ms to the client using TCP over port <25687>" << std::endl;
    SendAToMonitor(mon_soc, RecievedInputsFromClient,CalculatedDatasetA.dTrans,CalculatedDatasetA.dProp,CalculatedDatasetA.E2E );

  }

  if (int(ResultsFromB[0])!=0){
    SendForCompute(RecievedInputsFromClient, ResultsFromB, awsAsClient, serverC);
    struct CalculatedValuesFromC CalculatedDatasetB = recieveComputed(awsAsClient, &serverC, servC_len);
    std::cout << "The AWSB sent delay=<" << std::setprecision(2) << CalculatedDatasetB.E2E << ">ms to the client using TCP over port <25687>" << std::endl;
    SendBToMonitor(mon_soc, RecievedInputsFromClient,CalculatedDatasetB.dTrans,CalculatedDatasetB.dProp,CalculatedDatasetB.E2E );
    //sendData(mon_soc, monitor, RecievedInputsFcromClient);
    //SendToClient
    //SendToMonitor
  }




  //char *hello = "Hello from server";
  //send(mon_soc , hello , strlen(hello) , 0 );





    // if (((int(linkAVals[0])==0) && (int(linkBVals[0])==0))
    // {
    //   //send message to both client and monitor that nothing was found
    // }



}
