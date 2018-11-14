// use UDP port 22687
// use UDP port 21687


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
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#define AWS_SERVB 22687

int main(){
  printf("Server B is up and running\n");
  int awsSoc;
  struct sockaddr_in aws;
  //struct sockaddr_storage src_addr;
  //socklen_t src_addr_len=sizeof(src_addr);
  int addrlen = sizeof(int);
  int Vals[3];
  //memset(&hints,0,sizeof(hints));
  if((awsSoc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server B socket creation failed");
    return -1;
  }
  aws.sin_family = AF_INET;
  aws.sin_addr.s_addr = inet_addr("127.0.0.1");
  aws.sin_port = htons(AWS_SERVB);

  if (bind(awsSoc, (struct sockaddr *)&aws, sizeof aws) < 0)
  {
    perror("\nbind to socket failed");
    return -1;
  }
  int new_socket = accept(awsSoc, (struct sockaddr *)&aws,(socklen_t*)&addrlen);
  //recvfrom(awsSoc,Vals, 3*sizeof(int),0, (struct sockaddr*)&src_addr,&src_addr_len);
  recvfrom(awsSoc,Vals, 3*sizeof(int),0, (struct sockaddr*)&aws, (socklen_t *)&addrlen);
  printf("The Server B received input:%d\n", Vals[0]);

  std::stringstream x;
  x << Vals[0];
  std::string numberAsString(x.str());

  std::cout << "Checking for entry in db " << numberAsString << std::endl;

   std::ifstream databaseB ("database_b.csv");
   if(!databaseB.is_open()) std::cout << "Error: Couldn't open database" << std::endl;
   std::string link;

     std::string line, field;

     std::vector< std::vector<std::string> > dbB;  // the 2D array
     std::vector<std::string> dbBRows;                // array of values for one line only

     while (getline(databaseB,line))    // get next line in file
     {
         dbBRows.clear();
         std::stringstream ss(line);
         while (getline(ss,field,','))  // break line into comma delimitted fields
         {
             dbBRows.push_back(field);  // add each field to the 1D array
         }
         dbB.push_back(dbBRows);  // add the 1D array to the 2D array
     }

     // print out what was read in
     double dbValues[4];
     char *point;

     for (size_t i=0; i<dbB.size(); ++i)
     {
       if (dbB[i][0] == numberAsString)
       {
         for(int k=0; k<dbB[i].size(); k++)
         {
           const char * c = dbB[i][k].c_str();
           dbValues[k] =  strtod(c, &point);
           std::cout << dbValues[k] << "*\n";
          }
       }
     }

     if (sendto(awsSoc, dbValues, 4*sizeof(double), 0, (struct sockaddr *)&aws , sizeof(aws)) < 0){
       perror("failed to send\n");
       return -1;
     } else {
       printf("Sending link=%.2f aws\n", dbValues[0]);
   }

   databaseB.close();

}
