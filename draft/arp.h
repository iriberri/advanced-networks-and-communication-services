#include "eth.h"
#include "ipv4.h"
#include "ipv4_config.h"


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
int arp_resolve ( eth_iface_t * iface, ipv4_addr_t dest, mac_addr_t mac, ipv4_addr_t src);