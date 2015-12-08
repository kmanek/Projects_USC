#include "headers.h"

char *interface_port[4];
struct sockaddr_in source,dest;
int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j,sd,sd1,sd2;
uint8_t *src_mac, *dst_mac, *ether_frame,*octet;

void ProcessPacket(unsigned char* , int,long);
int print_icmp_packet(unsigned char* , int,long );

struct lookup *lp;
//struct ether_addr *ea;
struct ether_addr *ea_s;
struct ether_addr *ea_d;
struct iphdr *iph;
struct sockaddr_ll device;
int count =0;
uint32_t prev;

/////////first procedure//////
void *print_hello(void *threadid)
{
  long tid;
  tid = (long) threadid;
  int saddr_size , data_size;
  struct sockaddr saddr;
  struct  ifreq ifr_recv;

    unsigned char *buffer = (unsigned char *) malloc(2000); //Its Big!
    struct sockaddr_ll dev;
    memset(&ifr_recv,0, sizeof (ifr_recv));
    memset (&dev, 0, sizeof (dev));
    char *interface_recv;
    interface_recv=(char *)allocate_strmem(5);
    memcpy(interface_recv,interface_port[tid],4);


    int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;

    memcpy((char *)ifr_recv.ifr_name,(char *)interface_recv,IFNAMSIZ);

    dev.sll_family=AF_PACKET;
    dev.sll_protocol=htons(ETH_P_IP);
    if ((dev.sll_ifindex = if_nametoindex (interface_recv)) == 0) {
    perror ("if_nametoindex() failed to obtain interface index ");
    exit (EXIT_FAILURE);
  }

    if(bind(sock_raw,(struct sockaddr *)&dev, sizeof(dev))==-1)
    {
      perror("error to bind");
      exit(-1);
    }

    if(sock_raw < 0)
    {
        //Print the error with proper message
      perror("Socket Error");
      //return 1;
    }
    while(1)
    {
      saddr_size = sizeof saddr;
      data_size = recvfrom(sock_raw , buffer , 65536 , 0 , (struct sockaddr *)&dev , (socklen_t*)&saddr_size);
      if(data_size <0 )
      {
        printf("Recvfrom error , failed to get packets\n");
        //return 1;
      }
        //Now process the packet
      if((dev.sll_pkttype)==PACKET_HOST)
      {
        // printf("*******************NOT  OUTGOING PACKET************************\n");
        // continue;
        iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    // ++total;
    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;
    if (memcmp(inet_ntoa(dest.sin_addr),"192.168.252.1",4)==0){
                continue;
            }
        ProcessPacket(buffer , data_size,tid);
      }

      

    }
    close(sock_raw);
    printf("Finished");


  pthread_exit(NULL);
}
 

int main()
{
  //////////////////// from routing.c/////////////
      lp=(struct lookup *)malloc(sizeof(struct lookup));
      ea_s=(struct ether_addr *)malloc(sizeof(struct ether_addr)); 
      ea_d=(struct ether_addr *)malloc(sizeof(struct ether_addr)); 
      //ea=(struct ether_addr *)malloc(sizeof(struct ether_addr)); 
      //prev=(struct iphdr *)malloc(sizeof(struct iphdr));
      // src_mac = (uint8_t *)allocate_ustrmem (6);
      // dst_mac = (uint8_t *)allocate_ustrmem (6);
     // ether_frame = (uint8_t *)allocate_ustrmem (IP_MAXPACKET);
      octet= (uint8_t *)allocate_ustrmem (6);
  look_up=(struct lookup *)malloc(sizeof(struct lookup));
        memset (&device, 0, sizeof (device));

    //memset (&prev, 0, sizeof (prev));
    //prev->daddr=0xffffffff;
char eth_list[100]={0};
  if (read_arp(eth_list) < 0)
  {
    perror("Error opening file");
    return(-1);
  }
  eth_list[strlen(eth_list)-1]='\0';
  char duplicat_list[strlen(eth_list)];
  memcpy(duplicat_list,eth_list,strlen(eth_list));
  char *pch;int j=0;
  pch = strtok (eth_list," \t");
  interface_port[j++]=pch;
  while (pch != NULL)
  {
    pch = strtok (NULL, " \t");
    if (pch != NULL)
      interface_port[j++]=pch;
  }
  //////////////////////

  if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) 
   {
    perror ("socket() failed ");
    exit (EXIT_FAILURE);
  }

  ////////////from main.c///////
  pthread_t threads[NUM_THREADS];
  int rc;
  long t;
  for (t = 0; t < NUM_THREADS; t++)
  {
    rc = pthread_create(threads + t, NULL, print_hello, (void *) t);
    if (rc)
    {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  for (t = 0; t < NUM_THREADS; t++)
  {
    pthread_join(threads[t], NULL);
  }
  ///////////////////////////
  return 0;
  }

  void ProcessPacket(unsigned char* buffer, int size,long id)
  {
    //Get the IP Header part of this packet , excluding the ethernet header

    // printf("Starting ProcessPacket... of thread %ld\n",id);
    /*iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    // ++total;
    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;*/
    
    switch (iph->protocol) //Check the Protocol and do accordingly...
    {
        case 1:  //ICMP Protocol
        ++icmp;
        // printf("Starting switch...\n");
        print_icmp_packet( buffer , size,id);
        break;

        // case 2:  //IGMP Protocol
        // ++igmp;
        // break;


        case 17:
        ++udp;
        print_icmp_packet( buffer , size,id);
        break;


        default: //Some Other Protocol like ARP etc.
        ++others;
        break;
      }
      // printf("TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\n", tcp , udp , icmp , igmp , others , total);
    }

    
    int print_icmp_packet(unsigned char* Buffer , int Size, long id)
    {
      unsigned short iphdrlen;
      // struct ifreq ifr;                                                                 
      int bytes=0;
      //*ip_flags;
    //char *src_mac;
      
      // struct ip iphdr;
      // struct icmp icmphdr;
    
      
/////////////////////////////////////// Start lookup here/////////////////////////////////////////////
/*if (memcmp(inet_ntoa(dest.sin_addr),"192.168.252.1",4)==0){
                return;
            }*/
      if(iph->daddr!=prev)
      {
             //printf("hello\n");
              prev=iph->daddr;

            
            //printf("Before:::ETH %s\n\t MAC %s\n",lp->eth,lp->lookup_mac);
            if (memcmp(inet_ntoa(dest.sin_addr),"10.1.2.0",7)==0){
              if (memcmp(inet_ntoa(dest.sin_addr),"10.1.2.3",8)==0){
            // no need of next hop, just send by attaching the MAC, i.e. call ip_search() with some flag
                // printf("IT is 10.1.2.3\n");
                lp = (struct lookup *)ip_search(inet_ntoa(dest.sin_addr),3);
              }
              else{
            // no need of next hop, just send by attaching the MAC, i.e. call ip_search() with some flag
                // printf("IT is 10.1.2.4\n");
                lp = (struct lookup *)ip_search(inet_ntoa(dest.sin_addr),4);
              }
              // printf("!!!!!!!!!!!@@@@@@@@@@@@@@@####################$$$$$$$$$$$$$$$$$$$$$$$$^^^^^^^^^^^^^^^^^^^06\n" );
            }
            else{
          // call ip_search() without the flag, normal lookup
              // printf("Normal lookup\n");
              lp=(struct lookup *)ip_search(inet_ntoa(dest.sin_addr),1);
            }
             if ((device.sll_ifindex = if_nametoindex (lp->eth)) == 0) 
                {
                  perror ("if_nametoindex() failed to obtain interface index ");
                  exit (EXIT_FAILURE);
                }
                //printf("after loop\n");
char mac[20]={0};
                
                ea_d=(ether_aton(lp->lookup_mac));
    memcpy(octet,ea_d->ether_addr_octet,6);
   mac_lookup(lp->eth,mac);
    ea_s=(ether_aton(mac));



    }
      //printf("After::: ETH %s\n\t MAC %s\n",lp->eth,lp->lookup_mac);
/////////////////////////////////////// End lookup here/////////////////////////////////////////////
    

/*ea=(ether_aton(lp->lookup_mac));
   memcpy (Buffer, ea->ether_addr_octet, 6);
mac_lookup(lp->eth,mac);
ea=(ether_aton(mac));*/



	// Copy source MAC address.
    //char mac[20]={0};
    
/*printf("dewst mas\n");
    for (i=0; i<5; i++) 
    {
      printf ("%02x:", octet[i]);
   }
    printf ("%02x\n", octet[i]);
     printf("sor mac\n");
     for (i=0; i<5; i++) 
    {
      printf ("%02x:", ea_s->ether_addr_octet[i]);
   }
    printf ("%02x", ea_s->ether_addr_octet[i]);*/


     // Find interface index from interface name and store index in
    // struct sockaddr_ll device, which will be used as an argument of sendto().
    /*if (count == 0)
    {
      memset (&device, 0, sizeof (device));
      // count++;
    }*/ 
      //memset (&device, 0, sizeof (device));
      //printf("before  index %d\n",device.sll_ifindex );
     /* if ((device.sll_ifindex = if_nametoindex (lp->eth)) == 0) 
      {
        perror ("if_nametoindex() failed to obtain interface index ");
        exit (EXIT_FAILURE);
      }*/
     // printf("After  index %d\n",device.sll_ifindex );

     //struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr));
     /*iphdrlen = iph->ihl * 4;
     struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen  + sizeof(struct ethhdr));
     int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;*/
    //  if((unsigned int)(icmph->type) == 11)
    //  {
    //   printf(  "(TTL Expired)\n");
    // }
    // else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
    // {
    //   // printf( "  (ICMP Echo Reply)\n");
    // }

	// Modifications to the packet
	//Decrementing the TTL by 1 
    
    if (iph->ttl == 1)
    {
      // we need to create ICMP packet!!!!!!!
      create_icmp(Buffer ,Size, interface_port[id],sd);
      return;
    }
    // iph->ttl--;
    // iph->check=0;
    // iph->check=checksum((unsigned short *)iph,sizeof(struct iphdr));
// char *dmac="00:04:23:a6:57:76";
    // struct ether_addr *ea;
    //ea=(struct ether_addr *)malloc(sizeof(struct ether_addr)); 
    //ea=(ether_aton(lp->lookup_mac));
    // memcpy(dst_mac,ea->ether_addr_octet,6);
    //memcpy(device.sll_addr,ea->ether_addr_octet,6);
   // memcpy (Buffer, dst_mac, 6);
    
    //memcpy(dst_mac,ea->ether_addr_octet,6);

   memcpy (Buffer, octet, 6);
  
   
   // memcpy (Buffer + 6, src_mac, 6);
    
    memcpy (Buffer+6, ea_s->ether_addr_octet, 6);

    // Next is ethernet frame data (IPv4 header + ICMP header + ICMP data).

    // IPv4 header
  // memcpy (ether_frame + 12, Buffer+12, Size-12);

    // Submit request for a raw socket descriptor.
   
  /*if(setsockopt(sd , SOL_SOCKET , SO_BINDTODEVICE , lp->eth, strlen(lp->eth))<0)

  {
    perror("failed binding to the interface");
  }*/

    // Send ethernet frame to socket.
  if ((bytes = sendto (sd, Buffer, Size, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) 
  {
    perror ("sendto() failed");
    exit (EXIT_FAILURE);
  }
 // printf("************\n");
  return 1;
}

