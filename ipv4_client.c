#include "ipv4.h"
#include "eth.h"
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


/* int main ( int argc, char * argv[] );
 * 
 * DESCRIPCIÓN:
 *   Función principal del cliente IPv4. Lee los argumentos escritos
 *   en línea de comandos, envía datos a un servidor IPv4 y recibe
 *   su respuesta.
 *
 * PARÁMETROS:
 *   'argc': Número de parámetros pasados al programa por la línea de
 *           comandos, incluyendo el propio nombre del programa.
 *	argv[1]: Protocolo de la capa superior
 *	argv[2]: Dirección IPv4 destino
 *	argv[3]: Tamaño del payload del paquete IPv4 a rellenar
 * VALOR DEVUELTO:
 *   Código de salida del programa (-1= error).
 */
 int main ( int argc, char * argv[] )
 {

 	char * myself = basename(argv[0]);

 	if ((argc < 3) || (argc >4)){
 		printf("Uso: %s <Protocolo> <IP> <size>\n", myself);
 		printf("        <Protocolo>: Protocolo del paquete de la capa superior\n");
 		printf("         <IP>: Dirección destino del servidor IP\n");
 		printf("        <size>: Tamaño de payload\n");
 		printf("                (%d bytes por defecto)\n", IPv4_MAX_PAYLOAD_LENGTH);

 		exit(-1);
 	}

  /* Getting upper protocol */
 	unsigned char  type_protocol = atoi (argv[1]);

/*Getting destination IP (IPv4 server' IP)*/
 	ipv4_addr_t IP_dest;
 	int err_ip = ipv4_str_addr(argv[2], IP_dest);
 	if(err_ip==-1){
 		printf("Invalid target address  @ ipv4_client\n");
 		return -1;
 	}

/* Opening configuration file and routing table */
 	ipv4_open("ipv4_config_client.txt", "ipv4_route_table_client.txt");




 /* Create and fill the payload with consecutive numbers*/
 	unsigned char payload[IPv4_MAX_PAYLOAD_LENGTH];

 	int payload_len = atoi(argv[3]);
 	if ((payload_len < 0) || (payload_len > IPv4_MAX_PAYLOAD_LENGTH)) {
 		printf("Error in selected payload length (%d) @ ipv4_client\n", payload_len);
 		exit(-1);
 	}

 	int i;
 	for (i=0; i<payload_len; i++) {
 		payload[i] = (unsigned char) i;
 	}


  /* Send */
 	int err_send=ipv4_send (IP_dest, type_protocol, payload, payload_len);

 	if(err_send==-1){
 		printf("Error in ipv4_send() @ ipv4_client\n");
 		exit(-1);
 	}
 	if(err_send==0){
 		printf("Zero bytes sent @ ipv4_client\n");
 		exit(0);
 	}

 	printf("%d bytes sent. Receiving...  @ ipv4_client\n", err_send);


  /* Receive */

 	ipv4_addr_t IP_src; //IP from where the packet comes from
 	unsigned char buffer_rcvd[IPv4_MAX_PAYLOAD_LENGTH];
 	long int timeout=8000;

 	int err_rcvd= ipv4_receive  (IP_src, type_protocol, buffer_rcvd, timeout);

 	if(err_rcvd==-1){
 		printf("Error when receiving packet @ ipv4_client\n");
 		exit(-1);
 	}

 	if(err_rcvd==0){
 		printf("Timer expired when receiving  @ ipv4_client\n");
 		exit(0);
 	}

 	char IP_src_str[60];
 	ipv4_addr_str(IP_src, IP_src_str);
 	printf(" %d bytes where received from: %s @ ipv4_client\n", err_rcvd, IP_src_str);


/*     'packet': Puntero al contenido del paquete que se quiere imprimir.
 *    'pkt_len': Longitud total en bytes del paquete a imprimir.
 *    'hdr_len': Número de bytes iniciales que se quieren resaltar */
 print_pkt (buffer_rcvd,err_rcvd,0);
 
 return 0;
}
