#include <stdio.h>
#include <string.h>
#include <timerms.h>
#include <stdlib.h>
#include <libgen.h>

#include "udp.h"
#include "ipv4.h"
#include "rip.h"
#include "ripv2_route_table.h"


int get_entries_number(int total_size) {
  /* (Message - header) / size of an entry */
        return (total_size - RIP_HEADER_SIZE) / RIP_ENTRY_SIZE;
}

int send_table(ipv4_addr_t dst_ip) {
        /*We prepare the RIP packet to be sent back*/
        rip_packet response_packet;
        response_packet.command = RIP_RESPONSE;
        response_packet.version = RIP_VERSION;
        response_packet.zero = 0;

        /*Get the entries*/
        int c_route = 0;
        int c_route_rip = 0;
        for (c_route = 0; c_route < RIPv2_ROUTE_TABLE_SIZE; c_route++) {
                if (ripv2_route_table_get(rip_table, c_route) != NULL) {
                        ripv2_route_t *cur_route = (ripv2_route_table_get(rip_table, c_route));
                        if (cur_route->metric != 16) {
                                rip_entry cur_entry = rip_get_entry_from_rip_route(cur_route);
                                memcpy(&(response_packet.rip_entries[c_route_rip]), &cur_entry, 20);
                                c_route_rip++;
                        }
                }
        }

        int bsize = size_of_packet_for_rip_entries(c_route_rip);
        return udp_send(dst_ip, RIP_PORT, RIP_PORT, (unsigned char*) &response_packet, bsize);
}

void print_rip_entry(rip_entry *entry) {
        int family_id = ntohs(entry->family_id);
        int route_tag = ntohl(entry->route_tag);
        char ip_str[20];
        char msk_str[20];
        char hop_str[20];
        ipv4_addr_str(entry->ip, ip_str);
        ipv4_addr_str(entry->mask, msk_str);
        ipv4_addr_str(entry->nexthop, hop_str);

        printf("Family: %d\tTag: %d\tIP: %s\tMask: %s\tNext-Hop: %s\tMetric: %d\n", family_id, route_tag, ip_str, msk_str, hop_str, ntohl(entry->metric));
}


void print_rip_packet(rip_packet *packet, int size) {
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
                        if (time_passed == 0) { //Timer ha expirado. Aquí iria GARBAGE COLLECTOR
                                printf("Eliminando ruta que ha expirado: ");
                                rip_entry rip = rip_get_entry_from_rip_route(current_route);
                                print_rip_entry(&rip);
                                free(ripv2_route_table_remove(rip_table, i));
                        } //Timer ha expirado
                }
        }
}

int main ( int argc, char * argv[] )
{

  char * myself = basename(argv[0]);

/*  if ((argc) !=2) 
{
printf("Help: %s [<Port>]\n", myself);
exit(-1);
}
*/
char *conf_file = "ipv4_config_server.txt";
char * rout_file= "ipv4_route_table_server.txt";
// printf("%d port rip server\n", port);
ipv4_addr_t multicast_ip = { 224, 0, 0, 9 };



int payload_len = UDP_MAX_PAYLOAD_LENGTH;
if (argc == 5) 
{
  char * payload_len_str = argv[4];   
//printf("%d \n", n);



  udp_open(conf_file, rout_file, port);
ripv2_route_table_t *rip_table =  ripv2_route_table_create(); //New RIP table
ipv4_route_table_t *ipv4_table = ipv4_get_table(); //YEEEEY

ipv4_addr_t src_ip;
uint16_t  src_port  = 520;
unsigned char data[UDP_MAX_PAYLOAD_LENGTH];
long int timeout=-1;
ripv2_message_t received_packet;

/* Dump of static routes from IPV4 table*/

int c_route;
for (c_route = 0; c_route < IPv4_ROUTE_TABLE_SIZE; c_route++) {
  if (ipv4_route_table_get(table, c_route) != NULL) {
    ipv4_route_t *ip_cur_route = ipv4_route_table_get(table, c_route);
    timerms_t timer;
          timerms_reset(&timer, INFINITE_TIMEOUT); /* Static: never die*/
    ipv4_addr_t default_mask = { 0, 0, 0, 0 };
    if (memcmp(default_mask, ip_cur_route->subnet_addr, IPv4_ADDR_SIZE) != 0) {
      ripv2_route_table_add(
        rip_table,
        ripv2_route_create(ip_cur_route->subnet_addr, ip_cur_route->subnet_mask, ip_cur_route->iface, ip_cur_route->next_hop_addr, 0,
          timer));
    }
  }
}
/*SEND REQUEST OF ROUTES TO MULTICAST*/

rip_packet packet_req_mult;
packet_req_mult.command=RIP_REQUEST;
packet_req_mult.version=RIP_VERSION;
packet_req_mult.to_zero= 0;
rip_entry new_entry;
new_entry.addr_family=0;
new_entry.tag= 0;
memset(new_entry.ip, 0, IPv4_ADDR_SIZE);
memset(new_entry.subnet, 0 ,IPv4_ADDR_SIZE);
memset(new_entry.next_hop, 0, IPv4_ADDR_SIZE);
new_entry.metric=htonl(16);
packet_req_mult.rip_entries[0]=new_entry;

int s=udp_send(multicast_ip, src_port, packet_req_mult,sizeof(packet_req_mult));

if(s==-1)
{
  printf("Error sending first request @ripv2_server\n");
  exit(-1);
}




while(1)
{
/*Prepare buffer for receiving*/
  unsigned char r_buffer[UDP_MAX_SIZE];
  unsigned short int src_port;
  ipv4_addr_t source_ip;

/*Actual receiving*/
  int bsize = udp_receive(source_ip, &src_port, RIP_PORT, r_buffer, 5 * 1000);
  printf("Recibido paquete de tamaño: %d.\n", bsize);
  review_table_timers();
  if (bsize != 0) {
/*Dump UDP payload (a RIP packet)*/
    rip_packet received_packet;
    memcpy(&received_packet, r_buffer, bsize);
//int num_entries = get_entries_number(bsize);
    print_rip_packet(&received_packet, bsize);

/*Request or Response?*/
    if (received_packet.command == RIP_REQUEST) {
                  //Handle request.
      send_table(source_ip);

    } else if (received_packet.command == RIP_RESPONSE){
      int changed = 0;
      /*Response*/
      /*Iterate all entries*/
      int number_of_entries = get_entries_number(bsize);
      int current_entry = 0;
      for (current_entry = 0; current_entry < number_of_entries; current_entry++) {
        int idx = ripv2_route_table_find(rip_table, received_packet.rip_entries[current_entry].ip,
          received_packet.rip_entries[current_entry].mask);
        if (idx == -1) {
          changed = 1;
          /*Add the new route*/
          timerms_t new_route_timer;
          timerms_reset(&new_route_timer, RIP_TIMEOUT * 1000);
          ripv2_route_table_add(
            rip_table,
            ripv2_route_create(received_packet.rip_entries[current_entry].ip, received_packet.rip_entries[current_entry].mask, "",
              source_ip, ntohl(received_packet.rip_entries[current_entry].metric) + 1, new_route_timer));
          printf("Añadida ruta nueva:");
          print_rip_entry(&received_packet.rip_entries[current_entry]);

        } else {
          ripv2_route_t *route = ripv2_route_table_get(rip_table, idx);
         /*We check if the route comes from our current next hop for this route*/
          if (memcmp(source_ip, route->next_hop_addr, IPv4_ADDR_SIZE) == 0) {
            changed = 1;
            print_rip_entry(&received_packet.rip_entries[current_entry]);
            free(ripv2_route_table_remove(rip_table, idx));
            timerms_t new_route_timer;
            timerms_reset(&new_route_timer, RIP_TIMEOUT * 1000);
            int current_metric = ntohl(received_packet.rip_entries[current_entry].metric);
            if (current_metric != 16){
              current_metric++;
            }
            ripv2_route_table_add(
              rip_table,
              ripv2_route_create(received_packet.rip_entries[current_entry].ip, received_packet.rip_entries[current_entry].mask,
                "", source_ip, (current_metric), new_route_timer));
            break;
          }

          if (ntohl(received_packet.rip_entries[current_entry].metric) + 1 < route->metric) {
            changed = 1;
            /*Replace route: the new one has a better metric*/
            print_rip_entry(&received_packet.rip_entries[current_entry]);
            free(ripv2_route_table_remove(rip_table, idx));
            timerms_t new_route_timer;
            timerms_reset(&new_route_timer, RIP_TIMEOUT * 1000);
            ripv2_route_table_add(
              rip_table,
              ripv2_route_create(received_packet.rip_entries[current_entry].ip, received_packet.rip_entries[current_entry].mask,
                "", source_ip, ntohl(received_packet.rip_entries[current_entry].metric) + 1, new_route_timer));
          } else {
            if (memcmp(&source_ip, &(route->next_hop_addr), IPv4_ADDR_SIZE) == 0) {
              timerms_reset(&(route->timer), RIP_TIMEOUT * 1000);
              /*Updated route: update timer*/
              print_rip_entry(&received_packet.rip_entries[current_entry]);
            }
          }
        }
      }

      if (changed != 0)
        send_table(multicast_ip);
    }
    printf("----------------- Nueva tabla tras update: -------------------------\n");
    ripv2_route_table_print(rip_table);
  }


} 
udp_close(); 
return 0;
}
