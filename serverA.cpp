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

#define AWS_SERVA 21687

int main(){
  printf("Server A is up and running\n");
  int awsSoc;
  struct sockaddr_in aws;
  //struct sockaddr_storage src_addr;
  //socklen_t src_addr_len=sizeof(src_addr);
  int addrlen = sizeof(int);
  int Vals[3];
  //memset(&hints,0,sizeof(hints));
  if((awsSoc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server A socket creation failed");
    return -1;
  }
  aws.sin_family = AF_INET;
  aws.sin_addr.s_addr = inet_addr("127.0.0.1");
  aws.sin_port = htons(AWS_SERVA);

  if (bind(awsSoc, (struct sockaddr *)&aws, sizeof aws) < 0)
  {
    perror("\nbind to socket failed");
    return -1;
  }
  int new_socket = accept(awsSoc, (struct sockaddr *)&aws,(socklen_t*)&addrlen);
  //recvfrom(awsSoc,Vals, 3*sizeof(int),0, (struct sockaddr*)&src_addr,&src_addr_len);
  recv(awsSoc,Vals, 3*sizeof(int),0);
  shutdown(awsSoc, SHUT_RDWR);
  printf("The Server A received input:%d\n", Vals[0]);

 std::stringstream x;
 x << Vals[0];
 std::string numberAsString(x.str());

 std::cout << "Checking for entry in db " << numberAsString << std::endl;


  std::ifstream databaseA ("database_a.csv");

  if(!databaseA.is_open()) std::cout << "Error: Couldn't open database" << std::endl;
  std::string link;

    std::string line, field;

    std::vector< std::vector<std::string> > array;  // the 2D array
    std::vector<std::string> v;                // array of values for one line only

    while ( getline(databaseA,line) )    // get next line in file
    {
        v.clear();
        std::stringstream ss(line);

        while (getline(ss,field,','))  // break line into comma delimitted fields
        {
            v.push_back(field);  // add each field to the 1D array
        }

        array.push_back(v);  // add the 1D array to the 2D array
    }

    // print out what was read in

    for (size_t i=0; i<array.size(); ++i)
    {
        for (size_t j=0; j<array[i].size(); ++j)
        {
            std::cout << array[i][j] << "|"; // (separate fields by |)
        }
        std::cout << "\n";
    }

    std::cout << "myvector contains:";
    for (unsigned i=0; i<v.size(); i++)
      std::cout << ' ' << v[i];
    std::cout << '\n';

  databaseA.close();

}
