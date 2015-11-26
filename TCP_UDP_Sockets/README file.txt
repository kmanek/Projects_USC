NAME : KAPIL MANEK
-------------------------------------------------------
USC ID : 7547092538
-------------------------------------------------------
PROJECT DESCRIPTION
In this project we have simulated a server client model based on socket programming.
We simulated both the TCP and UDP socket scenarios.Out of the three phases used in the project the first two phases 
uses TCP socket whereas the last phase implements a UDP socket.
The first two phases involves communication between the healthcenterserver(server) and the two clients(patient 1 and 2)
via TCP socket.The last phase involves two servers(doctor1 and 2) which communicate with the above clients via UDP
socket after the first two phase are done with.
In phase 1 the clients pass their username and password to the server in order to get access grant to enter phase 2.
In phase 2  with the server sending the availability list to only the authentic clients,the client then requests
the appointment from the list.If the appointment is available the server sends the necessary information to client.
In phase 3 the UDP server gets insurance information from clients to which the server replies by sending the cost
of visit.
--------------------------------------------------------
CODE FILES DESCRIPTION
1.healthcenterserver.cpp
The code for this is taken from "Beej's	Guide to C Programming	and Network Programming	(socket	programming)".
The code uses static TCP port of 21538 to which the client connects.The server reads two files users.txt and 
availabilities.txt.The server first breaks both the files using delimiters to gain access of the contents in
the files and use it later.The server after getting the username and password from client compares it with the
stored database and authenticates the user.After successful authentication it sends the list of availabilities to the
client.After getting response from the client about selected appointment it sends doctor name and port number used for 
phase 3 by the client.The server closes the child socket for that client however the parent socket is still runs in infinite loop.

2.patient1.cpp & patient2.cpp
The code for this is taken from "Beej's	Guide to C Programming	and Network Programming	(socket	programming)".
Both this act as client and in phase 1 create a TCP socket and connect to healthcenterserver.Both the client load 
their respective patient#.txt file.The code then breaks the file into username and password.This information is 
then send to server.Failure in this authentication phase makes the client to terminate.Successful Authentication
then asks the client to enter the appoint they want from the list they get from server.After getting response from
server the clients use the information as its Static Port used in phase 3(UDP).The client in phase 3 loads its insurance name
from patient#insurance.txt file and sends it to the doctor(UDP server) allocated to it in phase 2.In response it gets
the cost from doctor.

3.doctor1.cpp and doctor2.cpp
The code for this is taken from "Beej's	Guide to C Programming	and Network Programming	(socket	programming)" but modified to
make it bidirectional.The code creates a UDP socket with Static port number.File doc#.txt is first read.After getting the insurance 
name from client it compares it with contents of doc#.txt and sends the cost of visit to respective client.However the connection
for other clients is still open.
-----------------------------------------------------------
Procedure to Execute
Step 1: In Availabilities.txt file change the port # of doc1 to 41538 and doc2 to 42538
Step 2: Open five putty terminals and execute the command "make -f Makefile.mak" without quotes on any terminal.
Step 3:Run healthcenterserver on 1st terminal by using ./h1 command
Step 4:Run doctor1 on 2nd terminal by using ./doc1 command
Step 5:Run doctor2 on 3rd terminal by using ./doc2 command
Step 6:Run patient1 on 4th terminal by using ./patient1 command
Step 7:Run patient2 on 5th terminal by using ./patient2 command
Step 8:Termination of healthcenterserver and doctor1 & 2 requires "Ctrl+c" to be pressed to break the loop
-----------------------------------------------------------
Idiosyncrasy	
Error in binding process will cause the project to fail
The clients are not concurrent and hence if one client is communicating with server the other has to wait to till first finishes to get connected.
------------------------------------------------------------
Reused code
I used some block of codes from "Beej's Guide to Network Programming" Some parts in phase 1,phase2 and phase 3, regarding how to create a socket,          *           
how to bind the socket with an address, how to sendto and recvfrom. These blocks of codes are identical to the ones used in Beej.The phase 3 however is made 
bidirectional by me and is not same entirely from above mentioned source.      
------------------------------------------------------------
THANK YOU !              




