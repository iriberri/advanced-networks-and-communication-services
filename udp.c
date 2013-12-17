#include "ipv4.h"
#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <timerms.h>
#include <string.h>

#define UPPER_PROTOCOL 17

struct udp_packet {
  uint16_t src_port;
  uint16_t dst_port;
  uint16_t length;
  uint16_t checksum;
  uint8_t payload[UDP_MAX_PAYLOAD_LENGTH];
};

typedef struct udp_packet udp_packet;

uint16_t port;


void udp_open( char * configf, char * routef, uint16_t p)
{
  port=p;
  printf("%d\n", port);
  ipv4_open(configf, routef);
  

}

int udp_send (ipv4_addr_t dst, uint16_t dest_port, unsigned char * payload, int payload_len )
{
  
  struct udp_packet packet;
  
  packet.src_port=htons(port);
  packet.dst_port=htons(dest_port);
  packet.length=htons(payload_len + UDP_HEADER_LENGTH);
  packet.checksum=0;
  memcpy(packet.payload, payload, payload_len);
 
  
  int e=0;
  e=ipv4_send ( dst, UPPER_PROTOCOL, (unsigned char *)&packet, payload_len+UDP_HEADER_LENGTH);
  
  if(e==-1)
  {
    printf("Error\n");
    return -1;
  }
  if(e==0)
  {
  
    printf("No data was sent\n");
    return 0;
  }
  
  return (e-UDP_HEADER_LENGTH);
}


int udp_recv (ipv4_addr_t src, uint16_t * src_port, unsigned char * data, long int timeout )
{  
    /* Inicializar temporizador para mantener timeout si se reciben tramas con
     tipo incorrecto. */
  timerms_t timer;
  timerms_reset(&timer, timeout);

  int frame_len;
  unsigned char buffer[IPv4_MAX_PAYLOAD_LENGTH];
  struct udp_packet* packet;

  do {
    long int time_left = timerms_left(&timer);

    frame_len = ipv4_receive (src, UPPER_PROTOCOL, buffer, time_left );
    //printf("Packet arrived at udp layer\n");
    if (frame_len == -1) {
      fprintf(stdout, "udp_recv(): ERROR en ipv4_recv(): \n");
      return -1;
    } 
    else if (frame_len == 0)
    {
#ifdef DEBUG
  printf("Timer expired");
#endif
      return 0;
    }
         packet=(struct udp_packet *) buffer;

//printf("%d, %d\n", port,ntohs(packet->dst_port));
  } while ( !(port==ntohs(packet->dst_port)));
  
  frame_len = ntohs(packet->length) - UDP_HEADER_LENGTH;
  memcpy(data, packet->payload, frame_len);
  uint16_t x=(ntohs(packet->src_port));
  memcpy(src_port, &x, 2);
  return (frame_len);
  
}  
  



void udp_close ()
{
  ipv4_close();
 
}

