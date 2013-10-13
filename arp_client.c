#include "arp.h"
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h> 
#include <string.h> 
#include <unistd.h> 
#include <netinet/in.h> 


int main ( int argc, char * argv[] )
{
  char * myself = basename(argv[0]);
  if (argc != 3) {
    printf("Uso: %s <iface> <inet>\n", myself);
    printf("       <iface>: Nombre de la interfaz Ethernet\n");
    printf("        <inet>: Dirección IP destino\n");
    exit(-1);
  }
  /* Processing arguments */
  char * iface_name = argv[1];
  char* dest_ip=argv[2];
  
  /* Variable that stores the IP addres to be asked for after the
  following conversion.*/
  ipv4_addr_t dest;
  
  int err=ipv4_str_addr(dest_ip, dest);
if (err==-1)
{
  printf("Error when converting IPv4 to string @arp_client\n");
}

  mac_addr_t target_addr; //This will be filled at arp_resolve
  memset(target_addr, 0, MAC_ADDR_SIZE);


  /* Opening Ethernet interface */
  eth_iface_t * iface = eth_open(iface_name);
  if (iface == NULL) {
printf("Error when opening ethernet interface (NULL) @arp_client\n");
    exit(-1);
  }

/* Call to arp_resolve function. As parameters we send the IP address
we are given to ask for, and the empty variable that will become the 
corresponding MAC address associated with the device that has the IP dest*/
int result_arp = arp_resolve (iface, dest, target_addr);
if (result_arp==-1){
  printf("Error: arp_resolve returns '-1' @arp_client\n");
	return -1;
}


/* Closing ethernet interface */
  
  //printf("Closing ethernet interface %s.\n", iface_name);
 // eth_close(iface);

return 0;

}

