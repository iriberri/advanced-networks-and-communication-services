#include <stdio.h>
#include <string.h>
#include <timerms.h>
#include <stdlib.h>
#include <libgen.h>

#include "udp.h"
#include "ipv4.h"
#include "rip.h"
#include "ripv2_route_table.h"



int main ( int argc, char * argv[] )
{
  
  char * myself = basename(argv[0]);
  
  if ((argc) !=2) 
  {
    printf("Help: %s [<Port>]\n", myself);
    exit(-1);
  }
  
  char *conf_file = "ipv4_config_server.txt";
  char * rout_file= "ipv4_route_table_server.txt";
  uint16_t port=(uint16_t) atoi(argv[1]);
  printf("%d port rip server\n", port);
  
  
  
  int payload_len = UDP_MAX_PAYLOAD_LENGTH;
  if (argc == 5) 
  {
    char * payload_len_str = argv[4];	  
    //printf("%d \n", n);

    payload_len = atoi(payload_len_str);
    if ((payload_len < 0) || (payload_len > IPv4_MAX_PAYLOAD_LENGTH)) {
      fprintf(stderr, "%s: Incorrect payload length: \"%s\"\n",
	      myself, payload_len_str);
      exit(-1);
    }
  }
  
  
  
  udp_open(conf_file, rout_file, port);
  //ripv2_route_table_t *rip_table =  ripv2_route_table_create();
  
  ipv4_addr_t src_ip;
  uint16_t  src_port;
  unsigned char data[UDP_MAX_PAYLOAD_LENGTH];
  long int timeout=-1;
  ripv2_message_t received_packet;
  
  while(1)
  {
    int len= udp_recv  (src_ip, &src_port, data,  timeout );  
    printf("Received message with length: %d. \n", len);
    
    if(len==-1)
    {
      printf("Error: Receiving data\n");
      exit(-1);
    }
    if (len!=-1 && len!=0)
    {
      memcpy(&received_packet, data, len);
    }
    
    if (received_packet.command == RIP_REQUEST){
      printf("Received request\n");
      
      
    }else if (received_packet.command == RIP_RESPONSE){
      printf("Received response\n");
      
      
    }
    
    
    
    /*SEND REQUEST*/
    
    // rip_packet packet;
    //   packet.command=RIP_REQUEST;
    //   packet.version=RIP_VERSION;
    //   packet.to_zero= 0;
    //   rip_entry new_entry;
    //   new_entry.addr_family=0;
    //   new_entry.tag= 0;
    //   memset(new_entry.ip, 0, IPv4_ADDR_SIZE);
    //   memset(new_entry.subnet, 0 ,IPv4_ADDR_SIZE);
    //   memset(new_entry.next_hop, 0, IPv4_ADDR_SIZE);
    //   new_entry.metric=htonl(RIP_INFINITY);
    //   packet.rip_entries[0]=new_entry;
    // 
    // int s=udp_send(src, src_port, packet,sizeof(packet) );
    //   if(s==-1)
    //   {
      //     printf("Error sending data\n");
      //     exit(-1);
      //   }
      // }
      //   
  } 
      udp_close(); 
      return 0;
}
