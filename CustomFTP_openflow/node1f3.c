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
int nack[PACKET_NUM];
struct negative
{
	int send_array[WINDOW];
	int size;
	int set;
}*neg;

struct timeval t1,t2;
// unsigned long int start,end;
// float total;
int start_nack,end_nack,j,k;

FILE *fp;
char *map;
int fd=0;

char *session;

void *send_nack(void *threadid)
	{
		sleep(8);
		long tid;
  		tid = (long) threadid;
		printf("Thread %ld in send\n",tid);
		char *interface="eth4",*dst_mac,*src_mac;
	
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
		neg=(struct negative *)malloc(sizeof(struct negative)); 
		// ether_frame=(uint8_t *)calloc(1,PACKET_SIZE*sizeof(uint8_t));
		memset(ether_frame,'b',PACKET_SIZE);
		// printf("PACKET: %s\n",ether_frame );

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
		// layer4 = (struct custom_transport *)(ether_frame  + sizeof(struct ethhdr) + 6 );
		neg=(struct negative *)(ether_frame  + HEADER_LEN - 4 );
		ea_src=ether_aton("a0:36:9f:08:56:c2");
		memcpy(eth->h_source,ea_src->ether_addr_octet,6);
		// eth->h_source=ea_src->ether_addr_octet;
		// eth->h_source=ea_src;
		// router-node1 interface
		// ea_dest=ether_aton("a0:36:9f:0a:5c:da");

		// router-node2 interface
		ea_dest=ether_aton("a0:36:9f:0a:0f:56");
		memcpy(eth->h_dest,ea_dest->ether_addr_octet,6);
		//eth->h_dest=ea_dest->ether_addr_octet;
		
		// eth->h_proto=0x0800;
		eth->h_proto=htons(ETH_P_IP);
		iph->src_ip=3;
		iph->dest_ip=2;
        iph->session_id=atoi(session);
		// iph->total_len=16;

		 // layer4->seq=1;
		// layer4->src_p=1;
		// layer4->dest_p=0;
		// layer4->type=1;
		/////NACK logic////////
		neg->set=1;
		memset (&device, 0, sizeof (device));
		if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
			perror ("if_nametoindex() failed to obtain interface index ");
			exit (EXIT_FAILURE);
		}
		while(1)
		{
		j=0;
		for (k = start_nack; k <= PACKET_NUM; k++)
		{
			if (nack[k]==0)
			{
				neg->send_array[j]=k;
				j++;
			}
			if (j== WINDOW)
				break;

		}
		neg->size=j;
		

		while(nack[start_nack]==1)
		{
			start_nack++;
		}
		if(start_nack>=PACKET_NUM)
		{
			neg->size=-1;

			int fin_count;
			for (fin_count =0; fin_count < 2 ;fin_count++)
			{
				if ((bytes = sendto (sock_raw, ether_frame, PACKET_SIZE, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) 
				{
					perror ("sendto() failed");
					exit (EXIT_FAILURE);
				}
			}
			// printf("LAST packet sent\n");
			break;
		}
			
		// if(start_nack>=end_nack)
		// {
		// 	end_nack=start_nack+WINDOW;
		// }

		// if(end_nack>PACKET_NUM)
		// {
		// 	end_nack=PACKET_NUM;
		// 	printf("nice\n");
		// }
		 printf("start_nack is : %d  and end_nack is : %d\n",start_nack,k);

		/////////END NACK LOGIC///////////////

		// memset (&device, 0, sizeof (device));
		// if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
		// 	perror ("if_nametoindex() failed to obtain interface index ");
		// 	exit (EXIT_FAILURE);
		// }
		// printf ("Index for interface %s is %i\n", interface, device.sll_ifindex);

		//int send_count=0;
		/*while(send_count <2)
		{*/
			// Send ethernet frame to socket.
			if ((bytes = sendto (sock_raw, ether_frame, PACKET_SIZE, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) 
			{
				perror ("sendto() failed");
				exit (EXIT_FAILURE);
			}

			// printf("Packet has been sent with nack_set %d\n",neg->set);
			neg->set++;
			//send_count++;
		usleep(1000*1000);
	  	}
		close(sock_raw);
		// printf("Send nack socket closed\n");
		pthread_exit(NULL);
		printf("Send nack thread exit\n");

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
		layer4 = (struct custom_transport *)malloc(sizeof( struct custom_transport));

		uint8_t *data;
		data=(unsigned char *)malloc(PACKET_SIZE - HEADER_LEN);
		//memset(&data,0,sizeof(data));
		memset (&dev, 0, sizeof (dev));
		char *buffer = (char *) malloc(PACKET_SIZE);
		//memset(buffer,0,sizeof(buffer));

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
		int data_size=0,i=0;
		// gettimeofday(&t1,0);
		// start=((t1.tv_sec)*1000000+t1.tv_usec);
		while(i<=PACKET_NUM)
		{

		  // printf("listening on thread ****\n");
		  up:data_size = recvfrom(sock_raw , buffer , PACKET_SIZE , 0 , (struct sockaddr *)&dev , (socklen_t*)&saddr_size);
		  if(data_size <=0 )
		  {
		    printf("Recvfrom error , failed to get packets\n");
		    exit(0);
		  }
		  iph = (struct ipheader *)(buffer + ETH_HDRLEN);
		  layer4 = (struct custom_transport *)(buffer  + HEADER_LEN - 4 );
		  if ((iph->src_ip<4) && (iph->session_id==atoi(session))) 
		  {
		     // memcpy(data,buffer + HEADER_LEN, PACKET_SIZE - HEADER_LEN);
		    printf("seq is :%d \n",layer4->seq );
              //printf("session  number is :%d \n",iph->session_id);
		    //////lab10//////////
		    if (nack[layer4->seq]==1)
		    	goto up;
		    ////////////////////
		     
		     if (layer4->seq==PACKET_NUM)
		    {
		    	// printf("LAST PACKET\n");
		    	int final_pkt = ( FILE_SIZE - (layer4->seq*(PACKET_SIZE - HEADER_LEN)));
		    	memcpy(map+(layer4->seq*(PACKET_SIZE - HEADER_LEN)),&buffer[HEADER_LEN],final_pkt);
		    }
		    else
		    {
		    	memcpy(map+(layer4->seq*(PACKET_SIZE - HEADER_LEN)),&buffer[HEADER_LEN],PACKET_SIZE - HEADER_LEN);
		    }
		    
		    // printf("SEQ %d\n",layer4->seq );
		    // printf("%d sadasd %d\n",start_nack,end_nack );
		    // printf("%d\n",i++ );
		    // nack[i]=layer4->seq;
		    // printf("NACK %d\n",nack[i] );
		    nack[layer4->seq]=1;
		    /*if (layer4->seq > end_nack)
		    {
		    	printf("Here\n");
		    	end_nack=layer4->seq;
		    }*/
		    i++;

		  }
		}
		gettimeofday(&t2,0);
		end=((t2.tv_sec)*1000000+t2.tv_usec);
		// total=((end-start));
		// printf("time to send : %f us  \n",total);
		printf("END Time : %ld\n", end);
		// printf("TIme elapsed: %ld microseconds\n",total );

		// printf("Finished\n");
		close(sock_raw);
		// printf("Socket Closed\n");
		pthread_exit(NULL);
		// printf("thread receive exit\n");
	}

int main(int argc, char *argv[]) {
	// your code goes here


fp=fopen("sachin_node1_from_3rx.bin","wb+");
	//exit(1);
	fd=fileno(fp);

	int result = lseek(fd,FILE_SIZE-1, SEEK_SET);
    if (result == -1) 
    {
		close(fd);
		perror("Error calling lseek() to 'stretch' the file");
		exit(EXIT_FAILURE);
    }
    
    result = write(fd, "", 1);
    if (result != 1) 
    {
		close(fd);
		perror("Error writing last byte of the file");
		exit(EXIT_FAILURE);
    }

    /* Now the file is ready to be mmapped.*/

    session=argv[1];
    printf("Session id = %s \n",session);
    // printf("Performing mmap to write\n");
    printf("PACKET_SIZE %d\n",PACKET_SIZE );
    map = mmap(0, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) 
    {
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
    }



	//Create threads
	end_nack = WINDOW;
	/*if (argc < 2)
	{
		printf("Wrong usage: \n");
		exit(1);
	}*/
	pthread_t threads[2];
  	int rc; long t=0;
	/*if (*argv[1]=='s')
	{*/
		// printf("SEND:\n");
		rc = pthread_create(threads+t , NULL, send_nack, (void *) t);
	    if (rc)
	    {
	      printf("ERROR; return code from pthread_create() is %d\n", rc);
	      exit(-1);
	    }
	/*}
	
	else if (*argv[1]=='r')
	{*/
		t++;
		rc = pthread_create(threads+t , NULL, receive, (void *) t);
	    if (rc)
	    {
	      printf("ERROR; return code from pthread_create() is %d\n", rc);
	      exit(-1);
	    }
	/*}
	else
	{
		printf("Wrong  usage in parameters: \n");
		exit(1);

	}*/

  for (t = 0; t < 2; t++)
  {
    pthread_join(threads[t], NULL);
    // printf("Joining thread %ld\n",t );
  }
	
	return 0;
}
