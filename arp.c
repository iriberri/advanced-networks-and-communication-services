#include "arp.h"
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <timerms.h> 
#include <unistd.h> 
#include <string.h> 
#include <unistd.h> 
#include <netinet/in.h> 


struct arp_packet{
 uint16_t hardw_type; //2bytes
  uint16_t  protocol_type; //2bytes
  uint8_t hdw_addr_len;  //1byte MAC length
  uint8_t  upp_proto_addr_len; //1byte (usually IPv4)
  uint16_t oper; //2bytes (1 for request, 2 for reply)
  mac_addr_t src_hrwd_addr; //6bytes
  ipv4_addr_t src_proto_addr; //4 bytes
  mac_addr_t dst_hrwd_addr; //6bytes
  ipv4_addr_t dst_proto_addr; //4bytes
};

typedef struct arp_packet arp_packet;

/* int arp_resolve ( eth_iface_t * iface, ipv4_addr_t dest, mac_addr_t mac)
 *
 * DESCRIPCIÓN: 
 *   Esta función devuelve la dirección MAC asociada a una IPv4 de un dispositivo
 *   conectado en red a la interfaz especificada.
 *
 * PARÁMETROS:
 *    'iface': Manejador de la interfaz Ethernet. 
 *   'dest': Dirección IPv4 que identifica al dispositivo del cual se quiere conocer
 *           su MAC.
 *   'mac': Variable en la que se guarda la dirección MAC obtenida.
 *
 * VALOR DEVUELTO:
 *   Se devuelve 1 si la función se ha ejecutado correctamente.
 *
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error.
 */
 int arp_resolve ( eth_iface_t * iface, ipv4_addr_t dest, mac_addr_t mac){

/*Generating ARP packet*/
 	arp_packet send_packet;

/*Array where the MAC address of the ETH interface will be copied. */
 	mac_addr_t mac_src;
 	eth_getaddr(iface, mac_src);

send_packet.hardw_type=htons(1); // Ethernet is 1
send_packet.protocol_type=htons(0x0800); //IP
send_packet.hdw_addr_len=MAC_ADDR_SIZE;
send_packet.upp_proto_addr_len=IPv4_ADDR_SIZE;
send_packet.oper=htons(1); //Specifies the operation that the sender is performing: 1 for request, 2 for reply
memcpy(send_packet.src_hrwd_addr,mac_src,MAC_ADDR_SIZE);
memset(send_packet.src_proto_addr,0,IPv4_ADDR_SIZE);
memset(send_packet.dst_hrwd_addr,0,MAC_ADDR_SIZE);
memcpy(send_packet.dst_proto_addr, dest ,IPv4_ADDR_SIZE);

/*
http://academic.keystone.edu/Cisco/Documents/Sem1/ccna1-Mod5-EthernetFrameTypes.pdf
Types: 
0x0800->IP Internet Protocol (IPv4) 
0x0806->Address Resolution Protocol (ARP)
0x86DD->Internet Protocol, Version 6 (IPv6)
*/


  /*Sending to broadcast the ARP request*/
 int err = eth_send(iface, MAC_BCAST_ADDR, 0x0806, (unsigned char *)&send_packet, sizeof(send_packet)); //CUIDADO EN IP
 if (err == -1) {
 	printf("Error when sending to broadcast. @ arp_resolve\n");
 	exit(-1);
 }   else{
 	printf("%d bytes have been sent. @ arp_resolve\n");
 }


 int len;
 arp_packet * rcvd_packet;
 unsigned char buffer[ETH_MTU];

  /* Receiving the reply */
 long int timeout = 2000;
 timerms_t timer;

 timerms_reset(&timer, timeout);
 do{
 	long int time_left=timerms_left(&timer);
 	/* mac id the MAC addres of the device that sent the frame. 
 	Is an output parameter where the address will be copied*/
 	len = eth_recv(iface, mac, 0x0806, buffer, time_left);
 	if (len == -1) {
 		printf("Error when receiving @ arp_resolve\n");
 		return -1;

 	} else if (len == 0) {
 		printf("The timer for reception has expired @ arp_resolve\n");
 		return 0;
 	}

 	rcvd_packet = (struct arp_packet *)buffer;

 	if (len > 0) {
//printf("hola");
 	}
/*Checking that the received packet is the expected one */
 }while(!(memcmp(rcvd_packet->src_proto_addr, dest, IPv4_ADDR_SIZE)==0 && ntohs(rcvd_packet->oper)==2));


 if (len > 0) {

 	memcpy(mac, rcvd_packet->src_hrwd_addr, MAC_ADDR_SIZE);
 	char src_addr_str[MAC_STR_LENGTH];
 	mac_addr_str(mac, src_addr_str); 
 	char src_ip_str[IPv4_STR_MAX_LENGTH];
 	ipv4_addr_str(dest, src_ip_str);

 	printf("%s -> %s \n",src_ip_str,src_addr_str);

 }


 return 1;

}

