#include <stdio.h>
#include <string.h>
#include <timerms.h>
#include <stdlib.h>
#include <libgen.h>

#include "udp.h"
#include "ipv4.h"
#include "rip.h"


#define UDP_PORT 520
#define RIP_REQUEST 1
#define RIP_RESPONSE 2
#define RIP_VERSION 2

#define MAX_RIP_ENTRIES 25
#define MAX_RIP_SIZE 504


/*
 * Variable Global empleada para 
 * manejar la dirección ip
 * a la que enviamos el request
 */
ipv4_addr_t router_ip;



int main (int argc, char * argv[])
{
    char * myself = basename(argv[0]);
  if(argc!=2)
  {
    printf("Uso: %s <IP server>\n", myself);

    exit(-1);
  }
  
  
  
    char * ip= argv[1];
  
  ipv4_str_addr(ip, router_ip);
  udp_open("ipv4_config_client.txt", "ipv4_route_table_client.txt", 1520);
  
  
  
  
   ripv2_message_t packet;
  packet.command=RIP_REQUEST;
  packet.version=RIP_VERSION;
  packet.zero= 0;
  ripv2_entry_t table_req;
  table_req.addr_family_id=0;
  table_req.route_tag= 0;
  memset(table_req.ip_addr, 0, IPv4_ADDR_SIZE);
  memset(table_req.mask, 0 ,IPv4_ADDR_SIZE);
  memset(table_req.next_hop, 0, IPv4_ADDR_SIZE);
  table_req.metric=htonl(16);
  packet.rip_entries[0]=table_req;
  
  int e= udp_send(router_ip, UDP_PORT, (unsigned char *)&packet, 24);
  
  return e;
  
}