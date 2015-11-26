#include "headers.h"

struct ipheader {
	unsigned int src_ip:16;
	unsigned int dest_ip:8;
    unsigned int session_id:8;
	// unsigned int total_len:16;
}*iph=NULL;

// struct custom_transport {
// 	unsigned int seq:13;
// 	unsigned int src_p:1;
// 	unsigned int dest_p:1;
// 	unsigned int type:1;
// }*layer4=NULL;

struct custom_transport {
	int seq;
}*layer4=NULL;

struct negative
{
	int rcv_ack[WINDOW];
	int size;
	int set;	
}*neg;

struct timeval t1,t2;

// float total;

FILE *fp;
char *map;
char *destination_mac;
char *session;
int fd=0;
int sock_raw_recv;

void *send_pkt(void *threadid)
	{
		long tid;
  		tid = (long) threadid;
		printf("Thread %ld in send\n",tid);
		char *interface="eth4",*dst_mac,*src_mac,*data="HELLO";
	
		// uint8_t *ether_frame;
		char ether_frame[PACKET_SIZE];
		struct sockaddr_ll device;
		struct ethhdr *eth=(struct ethhdr *)malloc(sizeof(struct ethhdr));
		struct ether_addr *ea_dest,*ea_src;
		int bytes=0;
		ea_dest=(struct ether_addr *)malloc(sizeof(struct ether_addr)); 
		ea_src=(struct ether_addr *)malloc(sizeof(struct ether_addr));
		src_mac=(char *)malloc(6*sizeof(char));
		dst_mac=(char *)malloc(6*sizeof(char));
		// ether_frame=(uint8_t *)calloc(1,PACKET_SIZE*sizeof(uint8_t));
		// memset(ether_frame,'a',PACKET_SIZE);
		memcpy(ether_frame + HEADER_LEN,map,PACKET_SIZE - HEADER_LEN);
		// ether_frame[PACKET_SIZE]='\0';
		

		// ether_frame = (uint8_t *)malloc(1500 * sizeof(uint8_t));
		// memset (&ether_frame, 0, (1500*sizeof(uint8_t)));
		iph = (struct ipheader *)malloc(sizeof( struct ipheader));

		
		// printf("%d\n", (int)sizeof(struct ipheader));
		
		layer4 = (struct custom_transport *)malloc(sizeof( struct custom_transport));
		
		// printf("%d\n", (int)sizeof(struct custom_transport));
	// **************************************************************************************************
		int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_IP)) ;
		if(sock_raw<0)
			perror("Socket cannot be created \n");
		eth = (struct ethhdr *)ether_frame;
		iph = (struct ipheader *)(ether_frame  + sizeof(struct ethhdr) );
		layer4 = (struct custom_transport *)(ether_frame  + HEADER_LEN - 4 );

		ea_src=ether_aton("a0:36:9f:0a:0f:56");
		memcpy(eth->h_source,ea_src->ether_addr_octet,6);
		// eth->h_source=ea_src->ether_addr_octet;
		// eth->h_source=ea_src;
		// router-node1 interface
		// ea_dest=ether_aton("a0:36:9f:0a:5c:da");

		// router-node2 interface
		ea_dest=ether_aton(destination_mac);
		memcpy(eth->h_dest,ea_dest->ether_addr_octet,6);
					
		//eth->h_dest=ea_dest->ether_addr_octet;
		
		// eth->h_proto=0x0800;
		eth->h_proto=htons(ETH_P_IP);
		iph->src_ip=2;
		iph->dest_ip=3;
        iph->session_id=atoi(session);
        
        printf("Session id = %d \n",iph->session_id);
		// iph->total_len=16;

		layer4->seq=0;
		// layer4->src_p=1;
		// layer4->dest_p=0;
		// layer4->type=1;

		memset (&device, 0, sizeof (device));
		if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
			perror ("if_nametoindex() failed to obtain interface index ");
			exit (EXIT_FAILURE);
		}
		// printf ("Index for interface %s is %i\n", interface, device.sll_ifindex);

	  // Send ethernet frame to socket.
		int i=0;
		gettimeofday(&t1,0);
		start=((t1.tv_sec)*1000000+t1.tv_usec);
		while(i<PACKET_NUM)
		{
			printf("SEQ : %d\n",layer4->seq);
			// printf("PACKET: %s\n",ether_frame+ HEADER_LEN );
			if ((bytes = sendto (sock_raw, ether_frame, PACKET_SIZE, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) 
			{
				perror ("sendto() failed");
				exit (EXIT_FAILURE);
			}
			// sendto (sock_raw, ether_frame, PACKET_SIZE, 0, (struct sockaddr *) &device, sizeof (device));
			i++;
			layer4->seq++;

			if (layer4->seq==PACKET_NUM)
		    {
		    	// printf("LAST PACKET*******************************%d***************************************\n",layer4->seq);
		    	int final_pkt = ( FILE_SIZE - (layer4->seq*(PACKET_SIZE - HEADER_LEN)));
		    	// memcpy(map+(layer4->seq*(PACKET_SIZE - HEADER_LEN)),&buffer[HEADER_LEN],final_pkt);
		    	memcpy(ether_frame + HEADER_LEN,map+(layer4->seq*(PACKET_SIZE - HEADER_LEN)) ,final_pkt);
		    	if ((bytes = sendto (sock_raw, ether_frame, PACKET_SIZE, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) 
					{
						perror ("sendto() failed");
						exit (EXIT_FAILURE);
					}
		    }
		    else
		    {
                //printf("i= %d\n", i);
		    	// memcpy(map+(layer4->seq*(PACKET_SIZE - HEADER_LEN)),&buffer[HEADER_LEN],PACKET_SIZE - HEADER_LEN);
		    	memcpy(ether_frame + HEADER_LEN,map+i*(PACKET_SIZE - HEADER_LEN) ,PACKET_SIZE - HEADER_LEN);
		    }

			// memcpy(ether_frame + HEADER_LEN,map+i*(PACKET_SIZE - HEADER_LEN) ,PACKET_SIZE - HEADER_LEN);

		}
		// gettimeofday(&t2,0);
		// end=((t2.tv_sec)*1000000+t2.tv_usec);
		// total=(float)((end-start));
		// printf("time to send : %f us  \n",total);
		printf("start time : %ld\n", start);
		// printf("TIme to end : %ld\n", end);

		// printf("Packet has been sent \n");
		close(sock_raw);
		pthread_exit(NULL);

	}

void *receive(void *threadid)
	{
		long tid;
  		tid = (long) threadid;
			printf("Thread %ld in receive\n",tid);
	 	struct sockaddr_ll dev;
		struct sockaddr saddr;
		int saddr_size;
		saddr_size = sizeof saddr;
		iph = (struct ipheader *)malloc(sizeof( struct ipheader));
		uint8_t *data;
		data=(unsigned char *)malloc(PACKET_SIZE - HEADER_LEN);
		//memset(&data,0,sizeof(data));
		memset (&dev, 0, sizeof (dev));
		char *buffer = (char *) malloc(PACKET_SIZE);
		//memset(buffer,0,sizeof(buffer));
		neg = (struct negative *)malloc(sizeof( struct negative));

		int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;
		if(sock_raw < 0)
		{
		      //Print the error with proper message
		  perror("Socket Error");
		      //return 1;
		}
		dev.sll_family=AF_PACKET;
		dev.sll_protocol=htons(ETH_P_IP);
		  //////give interface name
		if ((dev.sll_ifindex = if_nametoindex ("eth4") == 0)) {
		 perror ("if_nametoindex() failed to obtain interface index ");
		  exit (EXIT_FAILURE);
		}
		if(bind(sock_raw,(struct sockaddr *)&dev, sizeof(dev))==-1)
		{
		  perror("error to bind");
		  exit(-1);
		}
		  ///////////////////
		int data_size=0,bytes=0;
		struct ethhdr *eth=(struct ethhdr *)malloc(sizeof(struct ethhdr));
		struct ether_addr *ea_dest,*ea_src;
		ea_dest=(struct ether_addr *)malloc(sizeof(struct ether_addr)); 
		ea_src=(struct ether_addr *)malloc(sizeof(struct ether_addr));
		
		struct sockaddr_ll device_recv;
		memset (&device_recv, 0, sizeof (device_recv));
		char *interface="eth4";
		char ether_frame[PACKET_SIZE];
		if ((device_recv.sll_ifindex = if_nametoindex (interface)) == 0) {
			perror ("if_nametoindex() failed to obtain interface index ");
			exit (EXIT_FAILURE);
		}

		// ea_src=ether_aton("a0:36:9f:09:27:c6");
		// memcpy(eth->h_source,ea_src->ether_addr_octet,6);
	
		// ea_dest=ether_aton("a0:36:9f:0a:5c:d9");
		// memcpy(eth->h_dest,ea_dest->ether_addr_octet,6);
		
		// eth->h_proto=htons(ETH_P_IP);
		// iph->src_ip=2;
		// iph->dest_ip=3;
		sock_raw_recv = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_IP)) ;
		if(sock_raw_recv<0)
			perror("Socket cannot be created \n");
		// printf("sock_raw_recv %d\n",sock_raw_recv );
		while(1)
		{

		  // printf("listening on node2 \n");
		  data_size = recvfrom(sock_raw , buffer , PACKET_SIZE , 0 , (struct sockaddr *)&dev , (socklen_t*)&saddr_size);
		  if(data_size <=0 )
		  {
		    printf("Recvfrom error , failed to get packets\n");
		    exit(0);
		  }
		  iph = (struct ipheader *)(buffer + ETH_HDRLEN);
		  neg = (struct negative *)(buffer + HEADER_LEN-4);
		  

		  if ((iph->src_ip<4) && (iph->session_id==atoi(session))) 
		  {

		  	if (neg->size == -1)
		  {
		  	// printf("REC\n");
		  	break;
		  }
		    // memcpy(data,buffer + HEADER_LEN, PACKET_SIZE - HEADER_LEN);
		    // printf("Data is :%s on thread ****\n",data );
		    // printf("rcv size:%d and set value %d\n",neg->size,neg->set);
		    // printf("0: %d 1: %d 2: %d \n",neg->rcv_ack[0],neg->rcv_ack[1],neg->rcv_ack[2] );
		    int i;
		    // for (i=0;i<neg->size;i++)
		    // {
		    // 	printf("i= %d value= %d\n", i,neg->rcv_ack[i]);
		    // }
		  

				  ////////////////////////////////Start sending NACk//////////////////////////////////////////////
				  // printf("START SENDING NACK\n");
				  // char *interface="eth4";
			
					// uint8_t *ether_frame;
					// char ether_frame[PACKET_SIZE];
					
					// struct ethhdr *eth=(struct ethhdr *)malloc(sizeof(struct ethhdr));
					// struct ether_addr *ea_dest,*ea_src;
					
					// ea_dest=(struct ether_addr *)malloc(sizeof(struct ether_addr)); 
					// ea_src=(struct ether_addr *)malloc(sizeof(struct ether_addr));
					// ether_frame=(uint8_t *)calloc(1,PACKET_SIZE*sizeof(uint8_t));
					// memset(ether_frame,'a',PACKET_SIZE);
					// memcpy(ether_frame + HEADER_LEN,map,PACKET_SIZE - HEADER_LEN);
					// ether_frame[PACKET_SIZE]='\0';
					
					// iph = (struct ipheader *)malloc(sizeof( struct ipheader));

					
					// printf("%d\n", (int)sizeof(struct ipheader));
					
					// layer4 = (struct custom_transport *)malloc(sizeof( struct custom_transport));
					
					// printf("%d\n", (int)sizeof(struct custom_transport));
				// **************************************************************************************************
					// int sock_raw_recv = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_IP)) ;
					// if(sock_raw_recv<0)
					// 	perror("Socket cannot be created \n");
					eth = (struct ethhdr *)ether_frame;
					iph = (struct ipheader *)(ether_frame  + sizeof(struct ethhdr) );
              
              //printf("Session id = %s \n", iph->session_id);
					layer4 = (struct custom_transport *)(ether_frame  + HEADER_LEN - 4 );

					ea_src=ether_aton("a0:36:9f:0a:0e:1e");
					memcpy(eth->h_source,ea_src->ether_addr_octet,6);
				
					
						ea_dest=ether_aton(destination_mac);
						memcpy(eth->h_dest,ea_dest->ether_addr_octet,6);
					
					
					eth->h_proto=htons(ETH_P_IP);
					iph->src_ip=2;
					iph->dest_ip=3;
                    iph->session_id=atoi(session);
					// iph->total_len=16;

					// layer4->seq=0;
					
					// layer4->type=1;

					// memset (&device_recv, 0, sizeof (device_recv));
					// if ((device_recv.sll_ifindex = if_nametoindex (interface)) == 0) {
					// 	perror ("if_nametoindex() failed to obtain interface index ");
					// 	exit (EXIT_FAILURE);
					// }
					// printf ("Index for interface %s is %i\n", interface, device_recv.sll_ifindex);

				  // Send ethernet frame to socket.
					i=0;
					// gettimeofday(&t1,0);
					// start=((t1.tv_sec)*1000000+t1.tv_usec);
					while(i<neg->size)
					{
						// layer4->seq=1;
						// printf("PACKET: %s\n",ether_frame+ HEADER_LEN );
						layer4->seq=neg->rcv_ack[i];
						//printf("resend seq : %d\n",layer4->seq);
						memcpy(ether_frame + HEADER_LEN,map+neg->rcv_ack[i]*(PACKET_SIZE - HEADER_LEN) ,PACKET_SIZE - HEADER_LEN);
						if ((bytes = sendto (sock_raw_recv, ether_frame, PACKET_SIZE, 0, (struct sockaddr *) &device_recv, sizeof (device_recv))) <= 0) 
						{
							perror ("sendto() Failed");
							// printf("%d\n",sock_raw_recv );
							exit (EXIT_FAILURE);
						}
						// sendto (sock_raw_recv, ether_frame, PACKET_SIZE, 0, (struct sockaddr *) &device_recv, sizeof (device_recv));
						i++;
						// layer4->seq++;
						// memcpy(ether_frame + HEADER_LEN,map+neg->rcv_ack[i]*(PACKET_SIZE - HEADER_LEN) ,PACKET_SIZE - HEADER_LEN);

					}
					// gettimeofday(&t2,0);
					// end=((t2.tv_sec)*1000000+t2.tv_usec);
					// total=(float)((end-start));
					// printf("time to send : %f us  \n",total);
					// printf("TIme to start : %ld\n", start);

					// printf("NACK Packet has been sent \n");
					

				  ////////////////////////////////END sending NACk//////////////////////////////////////////////
			}
		}
		printf("Finished\n");
		close(sock_raw_recv);
		close(sock_raw);
		pthread_exit(NULL);
	}

int main(int argc, char *argv[]) {
	// your code goes here
	if(argc<3)
	{
		perror("less arguments");
	}

	if (atoi(argv[1])==2)
	{
	destination_mac="a0:36:9f:08:58:22";
	}
	else
	{
	destination_mac="a0:36:9f:08:56:c2";
	}
    session=argv[2];

	fp=fopen("sachin.bin","rb");
	// fp=fopen("sachin.txt","r");
    if(fp==NULL)
    {
      printf("file does not exist\n");
    }

	//Create threads
	// if (argc < 2)
	// {
	// 	printf("Wrong usage: \n");
	// 	exit(1);
	// }

    fd=fileno(fp);

	map = mmap(0, FILE_SIZE, PROT_READ, MAP_SHARED,fd, 0);
    if (map == MAP_FAILED) 
    {
	fclose(fp);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }


	pthread_t threads[2];
  	int rc; long t=0;
	// if (*argv[1]=='s')
	// {
		// printf("SND:\n");
		rc = pthread_create(threads+t , NULL, send_pkt, (void *) t);
	    if (rc)
	    {
	      printf("ERROR; return code from pthread_create() is %d\n", rc);
	      exit(-1);
	    }
	// }
	
	// else if (*argv[1]=='r')
	// {
		t++;
		rc = pthread_create(threads+t , NULL, receive, (void *) t);
	    if (rc)
	    {
	      printf("ERROR; return code from pthread_create() is %d\n", rc);
	      exit(-1);
	    }
	// }
	// else
	// {
		// printf("Wrong  usage in parameters: \n");
		// exit(1);

	// }

  for (t = 0; t < 2; t++)
  {
    pthread_join(threads[t], NULL);
  }
	
	return 0;
}
