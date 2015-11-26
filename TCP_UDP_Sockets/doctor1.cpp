#include <stdio.h>
#include<iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fstream>
#include<vector>
#include<typeinfo>
#include<stdlib.h>

using namespace std;
#define MYPORT "41538" // the port users will be connecting to
#define MAXBUFLEN 100
// get sockaddr, IPv4 or IPv6:
int z;
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(void)
{
    //reading file doc1.txt//
    ifstream file2( "doc1.txt", ios::binary ) ;
if(!file2)
    {
perror("Error opening file");
return(-1);
}
string str3;
vector<string> st1;
int i3=0;
while(!file2.eof())//loop used to break the doc1 file by delimiters \n and space
      {
          getline(file2,str3,'\n');
          i3+=2;
          int in = str3.find(" ");
          string p1 = str3.substr(0,in);
          string u1= str3.substr(in+1);
          st1.push_back(p1);
          st1.push_back(u1);
}
    //done reading
int sockfd;
struct addrinfo hints, *servinfo, *p;
int rv;
int numbytes;
struct sockaddr_in their_addr;
char buf[MAXBUFLEN],user[8];
socklen_t addr_len;
char s[INET6_ADDRSTRLEN];
memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
hints.ai_socktype = SOCK_DGRAM;
hints.ai_flags = AI_PASSIVE; // use my IP
if ((rv = getaddrinfo("localhost", MYPORT, &hints, &servinfo)) != 0) {
fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
return 1;
}
// loop through all the results and bind to the first we can
for(p = servinfo; p != NULL; p = p->ai_next) {
if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) {
perror("listener: socket");
continue;
}
if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
close(sockfd);
perror("listener: bind");
continue;
}
break;
}
if (p == NULL) {
fprintf(stderr, "listener: failed to bind socket\n");
return 2;
}
freeaddrinfo(servinfo);
addr_len = sizeof their_addr;
struct sockaddr_in sin;
socklen_t len = sizeof(sin);
//////loop used to get port number and IP address of doctor
if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
    perror("getsockname");
cout<<"Phase 3 : Doctor 1 has a static UDP port "<<MYPORT<<" and IP address "<<inet_ntoa(sin.sin_addr)<<endl;
while(1)
{
recvfrom(sockfd, buf, 10 , 0,
(struct sockaddr *)&their_addr, &addr_len);//receiving insurance name from patient
string buf_s(buf);
////loop to compare patients insurance with file doc1.txt and send cost if match occurs
for (int i=0;i<i3;i+=2)
{
    if (buf_s==st1[i])
       {
        sendto(sockfd, st1[i+1].c_str(),strlen(st1[i+1].c_str()) , 0,(struct sockaddr *)&their_addr, addr_len);//send  cost
           z=i;
       }
}
z=atoi(st1[z+1].c_str());
 recvfrom(sockfd, user, 8, 0,(struct sockaddr *)&their_addr, &addr_len);////receive username from patient
cout<<"Phase 3 : Doctor 1 receives the request from the patient with port number "<<(int)ntohs(their_addr.sin_port)<<" and the insurance plan "<<buf<<"."<<endl;
cout<<"Phase 3 : Doctor 1 has sent estimated price "<<z<<"$ to patient with port number "<<(int)ntohs(their_addr.sin_port)<<"."<<endl;
}
close(sockfd);
return 0;
}
