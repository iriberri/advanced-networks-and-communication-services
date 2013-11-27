#include "ipv4.h"
#include "eth.h"
#include "udp.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>


#ifndef DEBUG
#define DEBUG
#endif
/**/
extern eth_iface_t* iface;



/* int main ( int argc, char * argv[] );
 * 
 * DESCRIPCIÓN:
 *   Función principal del programa.
 *
 * PARÁMETROS:
 *   'argc': Número de parámetros pasados al programa por la línea de
 *           comandos, incluyendo el propio nombre del programa.
 *   'argv': Array con los parámetros pasado al programa por línea de
 *           comandos. 
 *           El primer parámetro (argv[0]) es el nombre del programa.
 *
 * VALOR DEVUELTO:
 *   Código de salida del programa.
 */
int main ( int argc, char * argv[] )
{

  /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
  char * myself = basename(argv[0]);
  
  if ((argc < 3) || (argc >5)) 
  {
    printf("Uso: %s <file> <cfile> [<Puerto>][<long>]\n", myself);
    printf("        <file>: Archivo de configuraci�n\n");
    printf("        <cfile>: Archivo de configuraci�n de rutas\n");
    printf("	    <Puerto>: Puerto en el que escucha el servidor\n");
    printf("        <long>: Longitud de los datos enviados al servidor IP\n");
    printf("                (%d bytes por defecto)\n", UDP_MAX_PAYLOAD_LENGTH);
  
    exit(-1);
  }

  /* Procesar los argumentos */
  char *conf_file =argv[1];
  printf("Leemos arvchivos de configuracion1\n");
  
  char * rout_file= argv[2];
  
  
  printf("Leemos arvchivos de configuracion2\n");

  uint16_t port=2345;

  if(argc>3)
  {
    port=(uint16_t) atoi(argv[3]);
    
  }
  
  #ifdef DEBUG
  printf("Leemos argumentos: %d %s %s\n", port,  conf_file, rout_file);
#endif
    
  int payload_len = UDP_MAX_PAYLOAD_LENGTH;
  if (argc == 5) 
  {
    char * payload_len_str = argv[4];
    payload_len = atoi(payload_len_str);
    if ((payload_len < 0) || (payload_len > IPv4_MAX_PAYLOAD_LENGTH)) {
      fprintf(stderr, "%s: Longitud de payload incorrecta: \"%s\"\n",
              myself, payload_len_str);
      exit(-1);
    }
  }
  
  int er=udp_open(conf_file, rout_file, port);
  
  if(er==-1)
    exit(-1);

  ipv4_addr_t src;
  uint16_t  src_port;
  unsigned char data[UDP_MAX_PAYLOAD_LENGTH];
  
while(1)
{
  
  long int timeout=-1;

  int len= udp_recv  (src, &src_port, data,  timeout );
  #ifdef DEBUG
    printf("Packet received\n");
    printf("Source port: %d\n", src_port); 
#endif

  

  if(len==-1)
  {
    printf("Error Receiving data\n");
    exit(-1);
  }
  printf("%d\n", len);

int s=udp_send(src, src_port, data,len );
  if(s==-1)
  {
    printf("Error sending data\n");
    exit(-1);
  }
#ifdef DEBUG
    printf("Packet sent\n");
    print_pkt ( data, s, 0 );
#endif
}
  

 
  return 0;
}
