#include "ipv4.h"
#include "eth.h"
#include "arp.h"
#include "ipv4_config.h"
#include "ipv4_route_table.h"
#include <timerms.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct ipv4_packet {
  uint8_t ip_version_length; //0x45
  uint8_t ip_tos; //Type of service Services (1B)
  uint16_t ip_len; //Datagram size (Header and payload) (1B)
  uint16_t ip_id; //Identification: NIAS
  uint16_t ip_off; //Fragment offset dont fragment: 0
  uint8_t ip_ttl; //Time to live: typically 64
  uint8_t ip_prot; //Transport layer protocol
  uint16_t ip_checksum; //Header checksum
  ipv4_addr_t ip_src; //4B Source IP address
  ipv4_addr_t ip_dst; //4B Destination IP address
  unsigned char payload[ETH_MTU - IPv4_HEADER_LENGTH]; //Payload
};

typedef struct ipv4_packet ipv4_packet;

eth_iface_t* iface;
ipv4_route_table_t * rt_table;
ipv4_addr_t my_ip_addr;
ipv4_addr_t netmask_src_addr;

/* ipv4_open (char *config_file, char *route_table);
 *
 * DESCRIPCI”N: 
 *   Esta funciÛn inicializa la interfaz Ethernet especificada en los archivos
 *   de configuraciÛn llamando a eth_open(), inicializa la tabla de rutas, y 
 *   obtiene la IP propia a travÈs de los archivos de configuraciÛn.
 *
 *   La memoria del manejador de interfaz devuelto debe ser liberada con la
 *   funciÛn 'ipv4_close()'.
 *
 * PAR¡METROS:
 *   'config_file': Nombre del fichero de configuraciÛn que se desea leer.
 *
 *   'route_table': Tabla de rutas.
 *
 * VALOR DEVUELTO:
 *   Ninguno
 */
 void ipv4_open(char * config_file, char * route_table){
 	int err_read;
 	char iface_nm[IFACE_NAME_LENGTH];
 	err_read= ipv4_config_read( config_file, iface_nm, my_ip_addr,  netmask_src_addr );
 	if (err_read!=0){
 		printf("Couldn't read configuration file \n");
 		exit(-1);
 	}
 	rt_table = ipv4_route_table_create();
 	int err_entries = ipv4_route_table_read(route_table, rt_table);  
  if(err_entries<1){
        printf("No entries read from route table\n");
  }

 	iface = eth_open(iface_nm);


  char myIP2[IPv4_STR_MAX_LENGTH];
  ipv4_addr_str(my_ip_addr, myIP2);
                printf("My ip taken of config: %s\n", myIP2);
 }
//inicializaciones: ficheros config (saber ip propia), interfaz, tabla rutas inicialziar y rellenar, abrir ethopen

/* int ipv4_close ();
 * 
 * DESCRIPCI”N:
 *   Esta funciÛn cierra la interfaz Ethernet y libera la memoria
 *   de su manejador. (Borra tabla de rutas)
 *
 * PAR¡METROS:
 *   Ninguno.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si la interfaz Ethernet se ha cerrado correctamente.
 * 
 * ERRORES:
 *   La funciÛn devuelve '-1' si se ha producido alg˙n error. 
 */
 void ipv4_close(){
 	eth_close(iface);
 	ipv4_route_table_free(rt_table);
 }
//liberar memoria, cerrar eth, borrar tabla rutas


/* int ipv4_send 
 * (ipv4_addr_t dst, int type, unsigned char * payload, int payload_len);
 *
 * DESCRIPCI”N:
 *   Esta funciÛn permite enviar un paquete IPv4 a la direcciÛn IPv4
 *   indicada.
 *
 * PAR¡METROS:
 *         'dst': DirecciÛn IP del equipo destino.
 *        'type': Valor del campo 'Tipo' de la trama Ethernet a enviar.
 *     'payload': Puntero a los datos que se quieren enviar en la trama
 *                Ethernet.
 * 'payload_len': Longitud en bytes de los datos a enviar.
 *
 * VALOR DEVUELTO:
 *   El n˙mero de bytes de datos que han podido ser enviados.
 * 
 * ERRORES:
 *   La funciÛn devuelve '-1' si se ha producido alg˙n error. 
 */
 int ipv4_send( ipv4_addr_t dst, uint8_t proto, unsigned char * payload, int payload_len){
 	ipv4_packet ipv4_packet;
 	ipv4_packet.ip_version_length=0x45;
 	ipv4_packet.ip_tos= 0x00;
 	ipv4_packet.ip_len=htons(IPv4_HEADER_LENGTH + payload_len);
 	ipv4_packet.ip_id=htons(0x2873);
 	ipv4_packet.ip_off= 0x0000;
 	ipv4_packet.ip_ttl= 64;
 	ipv4_packet.ip_prot= proto;
 	ipv4_packet.ip_checksum=0x0000;
  //checksum a 0 memset
 	memcpy(ipv4_packet.ip_dst, dst ,IPv4_ADDR_SIZE);
 	memcpy(ipv4_packet.ip_src, my_ip_addr ,IPv4_ADDR_SIZE);
 	memcpy(ipv4_packet.payload, payload, payload_len);
	
		 

  //inicializar campo checksum a 0 
  ipv4_packet.ip_checksum= htons(ipv4_checksum((unsigned char *)&ipv4_packet, IPv4_HEADER_LENGTH));//en udp, cabecera m·s payload, en ip sÛlo cabecera
  //sin payload len


    char st[IPv4_STR_MAX_LENGTH];
  ipv4_addr_str ( dst, st );
  printf("receiver IP addr %s\n", st);


  mac_addr_t dst_mac_addr; //6bytes
  
  ipv4_route_t * route;
  
  route=ipv4_route_table_lookup (rt_table, dst );
  if(route==NULL)
  {
  	printf("Required host unreachable after looking at route table @ipv4_send\n");
  	return -1;
  }
  //consultar tabla rutas lookup route table lookup
  
  /* route lookup:
   *  dir dest pertenece a ruta o no
   *  si pertenece, long de prefijo
   * longest prefix match
   * ip, subred, byte s byte and
   * host se queda a 0
   * comparar con dir de subred de ruta*/
//unsigned char zerobuf[IPv4_ADDR_SIZE] = { 0 };
char gateway_str[IPv4_STR_MAX_LENGTH];
  ipv4_addr_str(route->gateway_addr, gateway_str);
  //gateway == nexthop
   int check_gateway_zero= memcmp(gateway_str,"0.0.0.0",IPv4_STR_MAX_LENGTH);

   if (check_gateway_zero == 0){
   	int result_arp = arp_resolve (iface, route->gateway_addr, dst_mac_addr);
   	if (result_arp ==-1 ){
   		return -1;
   	}
   	if (result_arp ==0 ){
   		return -1;
   	}
    // next hop a arp

   }
  //next hop 00000
  //pasar dst a arp
   if (check_gateway_zero != 0){
   	int result_arp = arp_resolve (iface, dst, dst_mac_addr);
   	if (result_arp ==-1){
   		return -1;
   	}
   	if (result_arp ==0 ){
   		return -1;
   	}
   }

  int err = eth_send(iface, dst_mac_addr, IP_ETH_TYPE, (unsigned char *)&ipv4_packet,IPv4_HEADER_LENGTH + payload_len ); //CUIDADO EN IP
  if (err == -1){
  	printf("Error when sending packet @ipv4_send\n");
  	return -1;

  }else{
  	printf("%d bytes have been sent out of %d @ipv4_send\n", err, payload_len+IPv4_HEADER_LENGTH);

  }
  return err;
}

/* int ipv4_receive 
 * (ipv4_addr_t src, int type, unsigned char buffer[], long int timeout);
 *
 * DESCRIPCI”N:
 *   Esta funciÛn permite obtener el siguiente paquete IPv4 recibido por la
 *   interfaz Ethernet indicada. La operaciÛn puede esperar indefinidamente o
 *   un tiempo limitando dependiento del par·metro 'timeout'.
 *
 * PAR¡METROS:
 *      'src': DirecciÛn IP del equipo origen.
 *     'type': Valor del campo 'Tipo' de la trama Ethernet que se desea
 *             recibir. 
 *   'buffer': Array donde se almacenar·n los datos de la trama recibida.
 *             Deben reservarse al menos 'IPv4_IP_MAX_LENGTH_PAYLOADs' bytes para recibir dichos
 *             datos.
 *  'timeout': Tiempo en milisegundos que debe esperarse a recibir una trama
 *             antes de retornar. Un n˙mero negativo indicar· que debe
 *             esperarse indefinidamente, mientras que con un '0' la funciÛn
 *             retornar· inmediatamente, se haya recibido o no una trama.
 *
 * VALOR DEVUELTO:
 *   La longitud en bytes de los datos de la trama recibida, o '0' si no se ha
 *   recibido ninguna trama porque ha expirado el temporizador.
 * 
 * ERRORES:
 *   La funciÛn devuelve '-1' si se ha producido alg˙n error. 
 */
 int ipv4_receive( ipv4_addr_t src, uint8_t proto, unsigned char buffer[], long int timeout){
 	
     	printf("Starting send @ ipv4_receive\n");
	timerms_t timer;
 	timerms_reset(&timer, timeout);
 	unsigned char eth_buffer[ETH_MTU];
 	ipv4_packet packet;
 	int payload_len;
 	mac_addr_t mac_rcv;

 	do {
 		long int time_left = timerms_left(&timer);
		//printf("Inside do-while loop, waiting to receive something @ ipv4_server\n");
 		payload_len = eth_recv(iface, mac_rcv, IP_ETH_TYPE, eth_buffer, time_left);
 		if (payload_len == -1) {
 			printf("Packet cannot be received @ipv4_receive");
 			return -1;
 		}
 		if (payload_len == 0) {
 			printf("Timer expired at receiving @ipv4_receive");
 			return 0;
 		}

 		memcpy(&packet, eth_buffer, payload_len);

 		/*uint16_t checksum = ntohs(packet.ip_checksum);
 		packet.ip_checksum = 0;
 		uint16_t new_checksum = ipv4_checksum((unsigned char *) &packet, IPv4_HEADER_LENGTH);

 		if (new_checksum != checksum) {
 			printf("Incorrect checksum (%d!=%d)", checksum, new_checksum);
 		}*/
               // printf("Protocolo: %x\n", ntohs(packet.ip_prot));
                 //               printf("Protocolo sin ntohs: %d\n", packet.ip_prot);

       //         printf("Protocolo2: %d\n", proto);
		
		char IPdst[IPv4_STR_MAX_LENGTH];
  ipv4_addr_str( packet.ip_dst, IPdst);
  char myIP[IPv4_STR_MAX_LENGTH];
  ipv4_addr_str(my_ip_addr, myIP);
//printf("IP dest packet: %s\n", IPdst);
   //             printf("My ip (dst): %s\n", myIP);

           //     printf("Lo siguiente es un trozo de: \n");

		//print_pkt((unsigned char*) &packet.ip_prot, 2, 0);
 	} while (!((memcmp(packet.ip_dst,my_ip_addr, IPv4_ADDR_SIZE) == 0) && (packet.ip_prot) == proto));

 	memcpy(buffer, packet.payload, payload_len - IPv4_HEADER_LENGTH);
 	memcpy(src, packet.ip_src, IPv4_ADDR_SIZE);
	printf("A packet arrived with protocol number %d\n", packet.ip_prot);
  /* Return number of received bytes */
 	return ntohs(packet.ip_len);
 }


  /*
   * uint16_t ipv4_checksum ( unsigned char * data, int len )
   *
   * DESCRIPCI”N:
   *   Esta funciÛn calcula el checksum IP de los datos especificados.
   *
   * PAR¡METROS:
   *   'data': Puntero a los datos sobre los que se calcula el checksum.
   *    'len': Longitud en bytes de los datos.
   *
   * VALOR DEVUELTO:
   *   El valor del checksum calculado.
   */

   uint16_t ipv4_checksum ( unsigned char * data, int len )
   {
   	int i;
   	uint16_t word16;
   	unsigned int sum = 0;

    /* Make 16 bit words out of every two adjacent 8 bit words in the packet
     * and add them up */
   	for (i=0; i<len; i=i+2) {
   		word16 = ((data[i] << 8) & 0xFF00) + (data[i+1] & 0x00FF);
   		sum = sum + (unsigned int) word16;	
   	}

    /* Take only 16 bits out of the 32 bit sum and add up the carries */
   	while (sum >> 16) {
   		sum = (sum & 0xFFFF) + (sum >> 16);
   	}

    /* One's complement the result */
   	sum = ~sum;

   	return (uint16_t) sum;
   }


  /* void ipv4_addr_str ( ipv4_addr_t addr, char* str );
   * 
   * DESCRIPCI√ìN:
   *   Esta funci√≥n genera una cadena de texto que representa la direcci√≥n IPv4
   *   indicada.
   *
   * PAR√ÅMETROS:
   *   'addr': La direcci√≥n IP que se quiere representar textualente.
   *    'str': Memoria donde se desea almacenar la cadena de texto generada.
   *           Deben reservarse al menos 'IPv4_STR_MAX_LENGTH' bytes.
   */
   void ipv4_addr_str ( ipv4_addr_t addr, char* str )
   {
   	if (str != NULL) {
   		sprintf(str, "%d.%d.%d.%d",
   			addr[0], addr[1], addr[2], addr[3]);
   	}
   }

  /* int ipv4_str_addr ( char* str, ipv4_addr_t addr );
   * 
   * DESCRIPCI√ìN:
   *   Esta funci√≥n analiza una cadena de texto en busca de una direcci√≥n IPv4.
   *
   * PAR√ÅMETROS:
   *    'str': La cadena de texto que se desea procesar.
   *   'addr': Memoria donde se almacena la direcci√≥n IPv4 encontrada.
   *
   * VALOR DEVUELTO:
   *   Se devuelve 0 si la cadena de texto representaba una direcci√≥n IPv4.
   *
   * ERRORES:
   *   La funci√≥n devuelve -1 si la cadena de texto no representaba una
   *   direcci√≥n IPv4.
   */
   int ipv4_str_addr ( char* str, ipv4_addr_t addr )
   {
   	int err = -1;

   	if (str != NULL) {
   		unsigned int addr_int[IPv4_ADDR_SIZE];
   		int len = sscanf(str, "%d.%d.%d.%d", 
   			&addr_int[0], &addr_int[1], 
   			&addr_int[2], &addr_int[3]);

   		if (len == IPv4_ADDR_SIZE) {
   			int i;
   			for (i=0; i<IPv4_ADDR_SIZE; i++) {
   				addr[i] = (unsigned char) addr_int[i];
   			}

   			err = 0;
   		}
   	}

   	return err;
   }
