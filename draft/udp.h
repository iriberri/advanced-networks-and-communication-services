#define UDP_HEADER_LENGTH 8
#define UDP_MAX_PAYLOAD_LENGTH 1472 
#include <stdint.h>
#include <netinet/in.h>
#include "ipv4.h"
//1500 - 20 IP - 8 UDP

void udp_open(char * configf, char * routef, uint16_t p);
int udp_send (ipv4_addr_t dst, uint16_t dest_port, unsigned char * payload, int payload_len );
int udp_recv (ipv4_addr_t src, uint16_t  src_port, unsigned char * data, long int timeout );
void udp_close ();