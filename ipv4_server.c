#include "ipv4.h"
#include "eth.h"
#include <unistd.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

/* int main ( int argc, char * argv[] );
 * 
 * DESCRIPCIÓN:
 *   Función principal del servidor IPv4. Lee los argumentos escritos
 *   en línea de comandos, envía datos a un servidor IPv4 y recibe
 *   su respuesta.
 *
 * PARÁMETROS:
 *   'argc': Número de parámetros pasados al programa por la línea de
 *           comandos, incluyendo el propio nombre del programa.
 *	argv[1]: Protocolo de la capa superior
 * VALOR DEVUELTO:
 *   Código de salida del programa (-1= error).
 */
 int main ( int argc, char * argv[] )
 {
 	char * myself = basename(argv[0]);

 	if ((argc < 2) || (argc >3)) 
 	{
 		printf("Uso: %s <Protocolo>\n", myself);
 		printf("        <Protocolo>: Protocolo del paquete de la capa superior\n");
 		exit(-1);
 	}


 /* Getting upper protocol */
 	unsigned char  type_protocol = atoi (argv[1]);

 /* Opening configuration file and routing table */
 	ipv4_open("ipv4_config_server.txt", "ipv4_route_table_server.txt");


 	int payload_len = IPv4_MAX_PAYLOAD_LENGTH;
 	if (argc == 4) {
 		char * payload_len_str = argv[3];
 		payload_len = atoi(payload_len_str);
 		if ((payload_len < 0) || (payload_len > IPv4_MAX_PAYLOAD_LENGTH)) {
 			fprintf(stderr, "%s: Longitud de payload incorrecta: \"%s\"\n",
 				myself, payload_len_str);
 			exit(-1);
 		}
 	}

 	ipv4_addr_t IP_src;
 	unsigned char buffer[IPv4_MAX_PAYLOAD_LENGTH];

 	while(1){
  	printf("Starting server @ ipv4_server\n");
	  long int timeout=-1; //Infinite timeout

  int err_rcvd= ipv4_receive (IP_src, type_protocol, buffer, timeout);
  	printf("Something has been received @ipv4_server\n");


  if(err_rcvd==-1)
  {
  	printf("Error when receiving packet @ ipv4_server\n");
  	exit(-1);
  }
  
  if(err_rcvd==0)
  {
  	printf("Timer expired when receiving  @ ipv4_server\n");
  	exit(0);
  }

  char IP_src_str[60];
  ipv4_addr_str(IP_src, IP_src_str);
  printf(" %d bytes where received from: %s @ ipv4_server\n", err_rcvd, IP_src_str);


 /* Once we receive the packet, we sent it back */
  int err_send=ipv4_send ( IP_src, type_protocol, buffer, err_rcvd);
  
  if(err_send==-1)
  {
  	printf("Error in ipv4_send() @ ipv4_server\n");
  	exit(-1);
  }
  if(err_send==0)
  {
  	printf("Zero bytes sent @ ipv4_server\n");
  	exit(0);
  }

  printf("%d bytes sent back. @ ipv4_server\n", err_send);

}

return 0;
}
