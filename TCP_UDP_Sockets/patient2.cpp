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
#include<iostream>
#include <fstream>
#include<vector>
#include<typeinfo>
#define PORT "21538" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once
using namespace std;
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(int argc)
{
    const char *argv="localhost";
    //reading patient2.txt
    FILE *fp;
    vector<string> st;
string str;
char str1[1024];
int i=0,z;
//reading file patient2.txt
ifstream file( "patient2.txt", ios::binary ) ;
if(!file)
    {
perror("Error opening file");
return(-1);
}
while(!file.eof())
      {
          getline(file,str,'\n'); //splitting the file using \n and space delimiter
          i+=2;
          int index = str.find(" ");
          string p = str.substr(0,index);
          string u = str.substr(index+1);
          st.push_back(p);
          st.push_back(u);
}
//done reading of file patient2.txt
//reading file patiient2insurance.txt
    ifstream file2( "patient2insurance.txt", ios::binary ) ;
if(!file2)
    {
perror("Error opening file");
return(-1);
}
string str3,str4;
int in;
while(!file2.eof())
      {
          getline(file2,str3,'\r'); //splitting the file using \r
          in=str3.length();
          str4=str3.substr(0,in-1);
}
    //done reading of file patiient2insurance.txt
int sockfd, numbytes;
char buf[MAXDATASIZE];
struct addrinfo hints, *servinfo, *p;
int rv;
char s[INET6_ADDRSTRLEN];

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
if ((rv = getaddrinfo(argv, PORT, &hints, &servinfo)) != 0) {
fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
return 1;
}
// loop through all the results and connect to the first we can
for(p = servinfo; p != NULL; p = p->ai_next) {
if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) {
perror("client: socket");
continue;
}
if (int res=connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
close(sockfd);
perror("client: connect");
continue;
}
break;
}
if (p == NULL) {
fprintf(stderr, "client: failed to connect\n");
return 2;
}
inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
s, sizeof s);
freeaddrinfo(servinfo); // all done with this structure
//get the port number and IP address
struct sockaddr_in sin;
socklen_t len = sizeof(sin);
if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
    perror("getsockname");
//////////////////
//sending and receiving in phase 1
char buffer[1024],b1[1024],b2[1024],b3[1024],b4[2],b5[1024];

send(sockfd,"authenticate",12,0);//sending "authenticate" request to server
int res=recv(sockfd,b3,1024,0);//dummy receive as sending authenticate,username,password separately
    send(sockfd, st[0].c_str(), st[0].length(),0);//sending "username"  to server
    res=recv(sockfd,b1,1024,0);//dummy receive as sending authenticate,username,password separately
    send(sockfd,st[1].c_str(),st[1].length(),0);//sending "password"  to server
    res=recv(sockfd,b2,1024,0);//receiving the result of authentication phase "success" or "failure"
    cout<<"Phase 1 : Patient 2 has TCP port number "<<ntohs(sin.sin_port)<<" and IP address "<<inet_ntoa(sin.sin_addr)<<endl;
    cout<<"Phase 1 : Authentication request from Patient 2 with username "<<st[0]<<" and password "<<st[1]<<" has been sent to Health Center Server."<<endl;
    cout<<"Phase 1 : Patient 2 authentication result: "<<b2<<"."<<endl;
    ////////////
    string b2_s(b2);
    if (b2_s=="failure")//exit phase 1 if authentication fails
        return 0;
    cout<<"Phase 1 : End of Phase 1 for Patient 2."<<endl;
    //////phase 1 ends

    ///////phase 2 starts
    res=recv(sockfd,b3,1024,0);//receiving the availabilities from server
    vector<string> app(10);  //defining the vector to store the index number
    string avail(b3);
    int index5=avail.length();
    avail=avail.substr(0,index5-1);
    string b3_s(b3);
    int len1;
    len1=b3_s.length();
    size_t l = count(b3_s.begin(), b3_s.end(), '\n');//counting the number of \n in availabilities sent by server

        string in1,in2,in3,in4,in5,in6;
    int index2,index3,index4;
    //chopping of availabilities file and storing only index in vector app
    for (int k=0;k<l;k++)
    {
    index2=b3_s.find("\n");
    app[k]=b3_s.substr(0,index2);
    b3_s.erase(0,index2+1);
    index3=app[k].find(" ");
    app[k]=app[k].substr(0,index3);
    }

    cout<<"Phase 2 : The following appointments are available for Patient2:"<<endl<<avail<<endl;
    cout<<"Please enter the preferred appointment index and press enter:"<<endl;
    cin>>b4;
     bool f1=false;
    for (int u=0;u<20;u++)//loop used to compare the index entered by user to stored value in vector app
    {

            for (int k1=0;k1<l;k1++)
            {
                    if (b4==app[k1])
                        {
                          int r2=send(sockfd,b4,3,0);//send the index value to server only if it is listed in availibilities sent by server
                          f1=true;
                        }
            }
            if (f1==true)
                break;
            else //loop to re enter the index if its not listed in availabilities
            {
          cout<<"Please enter the preferred appointment index and press enter:"<<endl;
          cin>>b4;
            }
    }
    char phrase[1024];
    res=recv(sockfd,phrase,1024,0);
    string phrase_s(phrase);
    ///////if and else used to find out whether appointment is available or not
    if(phrase_s=="negative")
    {
        cout<<"Phase 2 : The requested appointment from Patient2 is not available.Exiting..."<<endl;
        return 0;
    }
    else{
            int start=phrase_s.find(" ");
            string doc=phrase_s.substr(0,start);
            string port=phrase_s.substr(start+1,9);
            cout<<"Phase 2 : The requested appointment is available and reserved to Patient2.The assigned doctor port number is "<<port<<"."<<endl;

////phase 2 ends

////phase 3 begins
memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_DGRAM;
if ((rv = getaddrinfo(argv, port.c_str(), &hints, &servinfo)) != 0) {
fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
return 1;
}
// loop through all the results and make a socket
for(p = servinfo; p != NULL; p = p->ai_next) {
if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) {
perror("talker: socket");
continue;
}
break;
}
if (p == NULL) {
fprintf(stderr, "talker: failed to bind socket\n");
return 2;
}
sendto(sockfd,str3.c_str(), strlen(str3.c_str()), 0,
p->ai_addr, p->ai_addrlen);////send insurance name to doctor
char buf1[3];
recvfrom(sockfd,buf1, 3, 0,NULL,NULL);//recieve cost of visit
sendto(sockfd,st[0].c_str(), strlen(st[0].c_str()), 0,p->ai_addr, p->ai_addrlen);//sending username to doctor
int z;
z=atoi(buf1);
struct sockaddr_in sin;
socklen_t len = sizeof(sin);
///// get the port number and IP address of patient2 in UDP phase
if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
    perror("getsockname");
inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
s, sizeof s);
cout<<"Phase 3 : Patient 2 has a dynamic UDP port number "<<ntohs(sin.sin_port)<<" and IP address "<<s<<endl;
cout<<"Phase 3 : The cost estimation request from Patient1 with insurance plan "<<str4<<" has been sent to the doctor with port number "<<port<<" and IP address "<<s<<endl;
cout<<"Phase 3 : Patient 2 receives "<<z<<"$ estimation cost from doctor with port number "<<port<<" and name "<<doc<<"."<<endl;
cout<<"Phase 3 : End of Phase 3 for Patient2."<<endl;
freeaddrinfo(servinfo);
    }
close(sockfd);
////end of phase 3
return 0;
}
