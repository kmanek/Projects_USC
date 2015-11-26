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
#define PORT "21538" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold
#include <fstream>
#include<vector>
#include<typeinfo>
#include<stdlib.h>

using namespace std;
//creating struct
struct abc
 {
     string in; string day;string t;string doc;string port;
 };
 /////////
 int a2=0;
void sigchld_handler(int s)
{
while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(void)
{
       //reading file users.txt
            FILE *fp;
    vector<string> st;
string str,str0,user,pass;
char s1[2] = " ";
char *token;
string a,b;
int i=0,z;
/* opening file for reading */
ifstream file("users.txt", ios::binary );
if(!file)
    {
perror("Error opening file");
return(-1);
}
while(!file.eof())
      {
          getline(file,str,'\n');
          i+=2;
          int index = str.find(" ");
          string p = str.substr(0,index);
          string u = str.substr(index+1);
          st.push_back(p);
          st.push_back(u);
      }
      //reading done of uers.txt
      //reading availabilities.txt
        int i2=0,start=0;
/* opening file for reading */
ifstream file1( "availabilities.txt", ios::binary ) ;
if(!file1)
    {
perror("Error opening file");
return(-1);
}
 struct abc a1;
vector<abc> store;
//loop used to split the file and store in structure abc with object a1
while(!file1.eof())
      {

          getline(file1,str0,'\n');

          int k=0;
          int index = str0.find(" ");
          a1.in = str0.substr(start,index);
          size_t pos = str0.find("\r");
          string str1=str0.substr(index+1,pos);
          index = str1.find(" ");
          a1.day = str1.substr(start,index);
         size_t pos1 = str1.find("\r");
          string str2=str1.substr(index+1,pos1);
          index = str2.find(" ");
          a1.t = str2.substr(start,index);
         size_t pos2 = str2.find("\r");
          string str3=str2.substr(index+1,pos2);
          index = str3.find(" ");
          a1.doc = str3.substr(start,index);
         size_t pos3 = str3.find("\r");
          string str4=str3.substr(index+1,pos2);
          index = str4.find(" ");
          a1.port = str4.substr(start,index);
        store.push_back(a1);//store all values of struct in vector store
        i2++;
      }
      //reading of file availabilities done
 int index1;
int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
struct addrinfo hints, *servinfo, *p;
struct sockaddr_in their_addr; // connector's address information
socklen_t sin_size;
struct sigaction sa;
int yes=1;
char s[INET6_ADDRSTRLEN];
char str7[INET_ADDRSTRLEN];
char buf1[1024];
int rv;
memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE; // use my IP
if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) {
fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
return 1;
}

// loop through all the results and bind to the first we can

for(p = servinfo; p != NULL; p = p->ai_next) {
if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) {
perror("server: socket");
continue;
}

if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
sizeof(int)) == -1) {
perror("setsockopt");
exit(1);
}
if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
close(sockfd);
perror("server: bind");
continue;
}

break;
}
if (p == NULL) {
fprintf(stderr, "server: failed to bind\n");
return 2;
}
freeaddrinfo(servinfo); // all done with this structure
if (listen(sockfd, BACKLOG) == -1) {
perror("listen");
exit(1);
}

struct sockaddr_in sin;
socklen_t len = sizeof(sin);
////get the port number and IP address of server
if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
    perror("getsockname");

sa.sa_handler = sigchld_handler; // reap all dead processes
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;
if (sigaction(SIGCHLD, &sa, NULL) == -1) {
perror("sigaction");
exit(1);
}
cout<<"Phase 1 : The Health Center Server has port number "<<PORT<<" and IP address "<<inet_ntoa(sin.sin_addr)<<endl;
while(1) { // main accept() loop
sin_size = sizeof their_addr;
new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
if (new_fd == -1) {
//perror("accept");
continue;
}
//get IP address of client
struct sockaddr_in sin;
socklen_t len = sizeof(sin);
inet_ntop(their_addr.sin_family,
get_in_addr((struct sockaddr *)&their_addr),
s, sizeof s);
//sending and receive of phase 1 starts
char user1[1024],pass1[1024],au1[1024];
memset(user1,0,1024);
memset(pass1,0,1024);
memset(au1,0,1024);
    int res=recv(new_fd, au1, 1024,0);//receive authenticate keyword from patient
    string au(au1);
    send(new_fd,"enter pass",100,0);//dummy send
    res = recv(new_fd, user1, 1024,0);//receive username
    string user(user1);
    send(new_fd,"enter password",100,0);//dummy send
    res=recv(new_fd,pass1,12,0);//receive password
    string pass(pass1);
    cout<<"Phase 1 : The Health Center Server has received request from a patient with username "<<user1<<" and password "<<pass1<<"."<<endl;
    bool f=true;
    //loop to compare username,password and authenticate
    for (int j=0;j<i;j+=2)
      {
        if ((au=="authenticate")&&(user.compare(st[j])==0)&&((pass==(st[j+1]))))
         {
             write(new_fd,"success",100);
             f=false;
            cout<<"Phase 1 : The Health Center Server sends the response success to patient with username "<<user<<"."<<endl;

         }

      }
         if (f==true){
                write(new_fd,"failure",100);
                 cout<<"Phase 1 : The Health Center Server sends the response failure to patient with username "<<user<<"."<<endl;
          f=true;
          close(new_fd);
          }
////////////loop for comparison ends
////phase 1 ends

//phase 2 begins
if (f==false)
{
cout<<"Phase 2 : The Health Center Server, receives a request for available time slots from patients with port no "<<(int)ntohs(their_addr.sin_port)<<" and IP address "<<s<<endl;
char fin[1024];
string row[1024];
//loop to append the first 3 columns of availabilities.txt
for (int i1=0;i1<(i2-1);i1++)
     {
      row[i1].append(store.at(i1).in).append(" ").append(store.at(i1).day).append(" ").append(store.at(i1).t);
      int abc=atoi(store.at(i1).in.c_str());
        if(a2!=abc)
         {
            row[6]+=row[i1]+"\n";
         }
     }
strcpy(fin,row[6].c_str());
send(new_fd,fin,1024,0);//send the first 3 columns of availabilities
cout<<"Phase 2 : The Health Center Sever sends availabe time slots to patient with username "<<user1<<"."<<endl;
char ind[10];
res=recv(new_fd,ind,10,0);//receiving the index for which patient wants the appointment
cout<<"Phase 2 : The Health Center Server receives a request for appointment "<<ind<<" from patient with port number "<<(int)ntohs(their_addr.sin_port)<<" and username "<<user<<"."<<endl;
index1=atoi(ind);
/////loop to check whether appointment is available or not
      if (index1==a2)
          {
               send(new_fd,"negative",1024,0);
               close(new_fd);
          }
              else
                {
              cout<<"Phase 2 : The Health Center Server confirms the following appointment "<<ind<<" to patient with username "<<user<<"."<<endl;
              a2=index1;
              row[7].append(store.at(index1-1).doc).append(" ").append(store.at(index1-1).port);//send doc name and port number
              send(new_fd,row[7].c_str(),10,0);
               }
}
////phase 2 ends
close(new_fd); // parent doesn't need this
}
return 0;
}
