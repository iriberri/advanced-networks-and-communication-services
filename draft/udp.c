#include "udp.h"
#include <stdlib.h>
#include "time.h"
#include "ipv4.h"
#include "eth.h"
#include <timerms.h>
#include <string.h>
#include <stdio.h>


//srand ((unsigned int) time(NULL));

//UDP header is 8B length

uint16_t port;


struct udp_packet {
  uint16_t src_port_number;
  uint16_t dst_port_number;
  uint16_t length_udp_header_data;
  uint16_t checksum;
  uint8_t payload[UDP_MAX_PAYLOAD_LENGTH];
};


typedef struct udp_packet udp_packet;



void udp_open(char * config_file, char * route_table, uint16_t port_nbr){
          
  ipv4_open(config_file, route_table);
  port = port_nbr;
  //numero random con semilla
  //int j;
  //                     j=1250+(int) (10.0*rand()/(RAND_MAX+1.0));

}

int udp_send (ipv4_addr_t dst, uint16_t dest_port, unsigned char * payload, int payload_len ){
 udp_packet udp_packet_send;
 
 udp_packet_send.src_port_number=htons(port);
 udp_packet_send.dst_port_number=htons(dest_port);
 udp_packet_send.length_udp_header_data=payload_len+UDP_HEADER_LENGTH;
 udp_packet_send.checksum=0;
 memcpy(udp_packet_send.payload, payload, payload_len);
  
 
 int err_ipsend=0;
  err_ipsend=ipv4_send ( dst, 33 , (unsigned char *)&udp_packet_send, payload_len+UDP_HEADER_LENGTH);
   
  if(err_ipsend==-1)
  {
  	printf("Error when sending packet @udp_send\n");
    return -1;
  }
  if(err_ipsend==0)
  {
   
  	printf("Any byte has been sent @udp_send\n");
    return 0;
  }
   
     printf("%d bytes have been sent out of %d @udp_send\n", err_ipsend, payload_len+UDP_HEADER_LENGTH);

  return (err_ipsend);
  
}

int udp_recv (ipv4_addr_t src, uint16_t src_port, unsigned char * data, long int timeout ){
	timerms_t timer;
 	timerms_reset(&timer, timeout);
 	unsigned char udp_buffer[IPv4_MAX_PAYLOAD_LENGTH];
 	udp_packet packet_send;
 	int payload_len;

 	do {
 		//long int time_left = timerms_left(&timer);
		payload_len = ipv4_receive ( src, 33, (unsigned char *)&packet_send, payload_len+UDP_HEADER_LENGTH);
		
 		if (payload_len == -1) {
 			printf("Packet cannot be received @udp_receive");
 			return -1;
 		}
 		if (payload_len == 0) {
 			printf("Timer expired at receiving @udp_receive");
 			return 0;
 		}

 		memcpy(&packet_send, udp_buffer, payload_len);

 	} while (!(ntohs(src_port)==port));

	
 	memcpy(udp_buffer, packet_send.payload, payload_len - UDP_HEADER_LENGTH);
	printf("A packet arrived with port number %d\n", packet_send.dst_port_number);
  /* Return number of received bytes */
 	return ntohs(packet_send.length_udp_header_data);
  
  
}
void udp_close (){
  ipv4_close();
  
}
