#include <stdio.h>
#include <string.h>
#include <timerms.h>
#include <stdlib.h>
#include <libgen.h>

#include "udp.h"
#include "ipv4.h"
#include "rip.h"
#include "ripv2_route_table.h"

#define UDP_PORT 520
#define RIP_REQUEST 1
#define RIP_RESPONSE 2
#define RIP_VERSION 2

#define MAX_RIP_ENTRIES 25
#define MAX_RIP_SIZE 504

void print_rip_entry(ripv2_entry_t *entry) {
	  int addr_family_id = ntohs(entry->addr_family_id);
	  int route_tag = ntohl(entry->route_tag);
	  char ip_str[20];
	  char msk_str[20];
	  char hop_str[20];
	  ipv4_addr_str(entry->ip_addr, ip_str);
	  ipv4_addr_str(entry->mask, msk_str);
	  ipv4_addr_str(entry->next_hop, hop_str);
	  
	  printf("Family: %d\tTag: %d\tIP: %s\tMask: %s\tNext-Hop: %s\tMetric: %d\n", addr_family_id, route_tag, ip_str, msk_str, hop_str, ntohl(entry->metric));
	}
	

	void print_rip_packet2(ripv2_message_t *packet, int size) {
	  size = size - 4; /*Header off*/
	  if (packet->command == 1) {
	    printf("Request RIPv2\n");
	  } else if (packet->command == 2){
	    printf("Response RIPv2\n");
	  }
	  int c_entry = 0;
	  while (size != 0) {
	    print_rip_entry(&(packet->rip_entries[c_entry]));
	    size = size - RIP_ENTRY_SIZE;
	    c_entry++;
	  }
	}
/*
 * Variable Global empleada para 
 * manejar la dirección ip
 * a la que enviamos el request
 */
ipv4_addr_t router_ip;

ripv2_entry_t rip_get_entry_from_rip_route(ripv2_route_t *rip_route) {
  ripv2_entry_t entry;
  entry.addr_family_id = htons(2);
  entry.route_tag = 0;
  memcpy(entry.ip_addr, rip_route->subnet_addr, 4);
  memcpy(entry.mask, rip_route->subnet_mask, 4);
  memset(entry.next_hop, 0, 4);
  entry.metric = htonl(rip_route->metric);
  return entry;
}

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
  long int timeout = -1;
  
  	unsigned char r_buffer[UDP_MAX_SIZE];
	unsigned short int src_port;
	ipv4_addr_t source_ip;
        int bsize = udp_recv(source_ip, &src_port, r_buffer, timeout);

	  if (bsize != 0) {
	/*Dump UDP payload (a RIP packet)*/
	ripv2_message_t received_packet;
	memcpy(&received_packet, r_buffer, bsize);
	//int num_entries = get_entries_number(bsize);
	print_rip_packet2(&received_packet, bsize);
	
	  }
	  ripv2_route_table_t *rip_table;

	  	  ripv2_route_table_read ( "rip_route_table.txt", rip_table);
		  ripv2_message_t response_packet;
		  
	    response_packet.command = RIP_RESPONSE;
	    response_packet.version = RIPV2_VERSION;
	    response_packet.zero = 0;
	    
	    /*Get the entries*/
	    int c_route = 0;
	    int c_route_rip = 0;
	    
	    for (c_route = 0; c_route < 256; c_route++) {
	      ripv2_route_t *cur_route = ripv2_route_table_get(rip_table, c_route);
	      //  printf("Entra en looop\n");
	      
	      if (cur_route != NULL) {
		printf("Entra en 222\n");
		
		//if (cur_route->metric != 16) {
		  ripv2_entry_t cur_entry = rip_get_entry_from_rip_route(cur_route);
		  memcpy(&(response_packet.rip_entries[c_route_rip]), &cur_entry, RIP_ENTRY_SIZE);
		  c_route_rip++;
		  // }
	    }
	  }



	
	int bsize2= RIP_HEADER_SIZE + (RIP_ENTRY_SIZE * c_route_rip);
	udp_send(router_ip, 1520, (unsigned char*) &response_packet, bsize2);

  return e;
  
}