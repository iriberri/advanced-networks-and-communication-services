#include <stdio.h>
#include <string.h>
#include <timerms.h>
#include <stdlib.h>
#include <libgen.h>

#include "udp.h"
#include "ipv4.h"
#include "rip.h"
#include "ipv4_route_table.h"
#include "ripv2_route_table.h"

ripv2_route_table_t *rip_table;


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

int get_entries_number(int total_size) {
  /* (Message - header) / size of an entry */
  return (total_size - RIP_HEADER_SIZE) / RIP_ENTRY_SIZE;
}


int send_table(ipv4_addr_t dst_ip, uint16_t dst_port) {
  /*We prepare the RIP packet to be sent back*/
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

// int i;
// int counter=0;
// for(i=0; i<25; i++){
	//        printf("cocococo\n");
	// 
	//  if (rip_table->routes[i] != NULL){
	//      printf("entra if\n");
	// 
	//   ripv2_entry_t new_entry;
	//   new_entry.addr_family_id=htons(AF_INET);
	//   new_entry.route_tag=0;
	//   memcpy(new_entry.ip_addr, rip_table->rip_entries[i]->subnet_addr, IPv4_ADDR_SIZE);
	//     memcpy(new_entry.mask, rip_table->rip_entries[i]->subnet_mask, IPv4_ADDR_SIZE);
	//   memcpy(new_entry.next_hop, rip_table->rip_entries[i]->next_hop, IPv4_ADDR_SIZE);
	// 
	//    counter++;
	//    printf("counter %d\n", counter);
	//    
	//   
	//  }
	
	
	
	
	//    for (i = 0; i < RIPv2_ROUTE_TABLE_SIZE; i++) {
	//                 ripv2_route_t * route_i = ripv2_route_table_get(rip_table, i);
	//                 if (route_i != NULL) {
	// 		  ripv2_entry_t new_entry;
	// 		    new_entry.addr_family_id=htons(AF_INET);
	// 		  new_entry.route_tag=0;
	// 		  memcpy(new_entry.ip_addr, route_i->subnet_addr, IPv4_ADDR_SIZE);
	// 		  memcpy(new_entry.mask, route_i->subnet_mask, IPv4_ADDR_SIZE);
	// 		  memcpy(new_entry.next_hop, route_i->next_hop, IPv4_ADDR_SIZE);
	// 		  
	// 
	//                         new_entry.iface = route_i->iface;
	//                         new_entry.metric = route_i->metric;
	//                         new_entry.timer = timerms_left(&(route_i->timer));
	// 			
	// 					  memcpy(route_i, response_packet->routes[i], 20);
	// 
	//   
	// }
	
	int bsize = RIP_HEADER_SIZE + (RIP_ENTRY_SIZE * c_route_rip);
	return udp_send(dst_ip, dst_port, (unsigned char*) &response_packet, bsize);
	}
	
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
	
	
	void print_rip_packet(ripv2_message_t *packet, int size) {
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
	
	
	void review_table_timers() {
	  int i = 0;
	  int time_passed;
	  ripv2_route_t *current_route;
	  for (i = 0; i < RIPv2_ROUTE_TABLE_SIZE; i++) {
	    current_route = ripv2_route_table_get(rip_table, i);
	    if (current_route != NULL) {
	      time_passed = timerms_left(&current_route->timer);
	      if (time_passed == 0) { 
		printf("Eliminando ruta que ha expirado: ");
		ripv2_entry_t rip = rip_get_entry_from_rip_route(current_route);
		print_rip_entry(&rip);
		free(ripv2_route_table_remove(rip_table, i));
	      } //Timer ha expirado
	    }
	  }
	}
	
	
	
	int main ( int argc, char * argv[] )
	{
	  
	  //char * myself = basename(argv[0]);
	  
	  /*  if ((argc) !=2) 
	   * {
	   * printf("Help: %s [<Port>]\n", myself);
	   * exit(-1);
	   }
	   */
	  char *conf_file = "ipv4_config_server.txt";
	  char * rout_file= "ipv4_route_table_server.txt";
	  // printf("%d port rip server\n", port);
	  ipv4_addr_t multicast_ip = { 224, 0, 0, 9 };
	  
	  
	  
	  
	  
	  
	  udp_open(conf_file, rout_file, 520);
	  rip_table =  ripv2_route_table_create(); //New RIP table
	  //ipv4_route_table_t *ipv4_table = ipv4_get_table(); //YEEEEY
	  
	  //ipv4_addr_t src_ip;
	  uint16_t  src_port  = 520;
	  //unsigned char data[UDP_MAX_PAYLOAD_LENGTH];
	  //long int timeout=-1;
	  
	  printf("HEY\n");
	  ripv2_route_table_read ( "rip_route_table.txt", rip_table);
      ripv2_route_table_print(rip_table);
      
      
      /*SEND REQUEST OF ROUTES TO MULTICAST*/
      
      ripv2_message_t packet_req_mult;
      packet_req_mult.command=RIP_REQUEST;
      packet_req_mult.version=RIPV2_VERSION;
      packet_req_mult.zero= 0;
      ripv2_entry_t new_entry;
      new_entry.addr_family_id=0;
      new_entry.route_tag= 0;
      memset(new_entry.ip_addr, 0, IPv4_ADDR_SIZE);
      memset(new_entry.mask, 0 ,IPv4_ADDR_SIZE);
      memset(new_entry.next_hop, 0, IPv4_ADDR_SIZE);
      new_entry.metric=htonl(16);
      packet_req_mult.rip_entries[0]=new_entry;
      
      //int s=udp_send(multicast_ip, src_port, (unsigned char *)&packet_req_mult, sizeof(packet_req_mult));
      
      // if(s==-1)
      // {
	//   printf("Error sending first request @ripv2_server\n");
      //   exit(-1);
      // }
      
      
      long int timeout =-1;
      double send_resp_time = 25000;
      
      while(1)
      {
	/*Prepare buffer for receiving*/
	unsigned char r_buffer[UDP_MAX_SIZE];
	unsigned short int src_port;
	ipv4_addr_t source_ip;
	/*Compute minimum time*/
	int i;
	for(i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++){
	  if (rip_table->routes[i] != NULL){
	    long  int time_left = timerms_left(&(rip_table->routes[i]->timer));
	    if (timeout < 0){
	      timeout = time_left;
	    } else if (time_left <0) {
	      //DO nothing
	    }else if (timeout>time_left){
	      timeout = time_left;
	    }
	  }
         }
      
      /*Actual receiving*/
      int bsize = udp_recv(source_ip, &src_port, r_buffer, timeout);
      printf("Received packet. Size: %d.\n", bsize);
      //i bsize = 0
      if(bsize == 0){
	review_table_timers();
      }
      if (bsize != 0) {
	/*Dump UDP payload (a RIP packet)*/
	ripv2_message_t received_packet;
	memcpy(&received_packet, r_buffer, bsize);
	//int num_entries = get_entries_number(bsize);
	print_rip_packet(&received_packet, bsize);

	/*Request or Response?*/
	if (received_packet.command == RIP_REQUEST) {
	  //Handle request.
	  printf("Received request\n");
	  send_table(source_ip, src_port);
	  
	} else if (received_packet.command == RIP_RESPONSE){
	  int changed = 0;
	  /*Response*/
	  /*Iterate all entries*/
	  int number_of_entries = get_entries_number(bsize);
	  int current_entry = 0;
	  for (current_entry = 0; current_entry < number_of_entries; current_entry++) {
	    int current_metric = ntohl(received_packet.rip_entries[current_entry].metric);
		if (current_metric != 16){
		  current_metric++;
		}
	    int idx = ripv2_route_table_find(rip_table, received_packet.rip_entries[current_entry].ip_addr,
					     received_packet.rip_entries[current_entry].mask);
	    if (idx == -1 && current_metric<16) {
	      changed = 1;
	      /*Add the new route*/
	      
	      ripv2_route_table_add(
		rip_table,
		ripv2_route_create(received_packet.rip_entries[current_entry].ip_addr, received_packet.rip_entries[current_entry].mask, "",
				   source_ip, ntohl(received_packet.rip_entries[current_entry].metric) + 1, RIP_TIMEOUT * 1000));
	      printf("Añadida ruta nueva:");
	      print_rip_entry(&received_packet.rip_entries[current_entry]);
	      
	    } else {
	      ripv2_route_t *route = ripv2_route_table_get(rip_table, idx);
	      /*We check if the route comes from our current next hop for this route*/
	      if (memcmp(source_ip, route->next_hop, IPv4_ADDR_SIZE) == 0) {
		changed = 1;
		print_rip_entry(&received_packet.rip_entries[current_entry]);
	
		route->metric = current_metric;
		timerms_reset(&route->timer, RIP_TIMEOUT*1000);	
	      }else{
	      
	      if (current_metric < route->metric) {
		changed = 1;
		/*Replace route: the new one has a better metric*/
		print_rip_entry(&received_packet.rip_entries[current_entry]);
				memcmp(route->next_hop, source_ip, IPv4_ADDR_SIZE);

		route->metric = current_metric;
		timerms_reset(&route->timer, RIP_TIMEOUT*1000);
		
		
	      } else {
		if (memcmp(&source_ip, &(route->next_hop), IPv4_ADDR_SIZE) == 0) {
		  timerms_reset(&(route->timer), RIP_TIMEOUT * 1000);
		  /*Updated route: update timer*/
		  print_rip_entry(&received_packet.rip_entries[current_entry]);
		}
	      }
	    }
	  }
	  
	  if (changed != 0)
	    send_table(multicast_ip, RIP_PORT);
	}
	printf("======================= RIPv2 TABLE =======================\n");
	ripv2_route_table_print(rip_table);
      }
      
      
	} 
	
      }
	udp_close(); 
	return 0;
	}
	