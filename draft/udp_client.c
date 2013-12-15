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
  
  if ((argc < 4) || (argc >6)) 
  {
    printf("Uso: %s <Puerto> <IP> <file> [<long>]\n", myself);
    printf("        <Puerto>: Puerto de destino\n");
    printf("         <IP>: Dirección del servidor IP\n");
    printf("        <file>: Archivo de configuraci�n\n");
    printf("	    <cfile>: Archivo de configuraci�n de rutas\n");
    printf("        <long>: Longitud de los datos enviados al servidor UDP\n");
    printf("                (%d bytes por defecto)\n", UDP_MAX_PAYLOAD_LENGTH);
  
    exit(-1);
  }

  /* Procesar los argumentos */
  uint16_t  dest_port = atoi (argv[1]);

  char* ipv4_str = argv[2];


  ipv4_addr_t IP_dest;
  int err =ipv4_str_addr ( ipv4_str, IP_dest);
  if(err==-1)
  {
    printf("Invalid destination address\n");
    return -1;
  }
  
  char * conf_file= argv[3];
  
  char * rout_file=argv[4];
  
#ifdef DEBUG
  printf("Leemos argumentos: %d %s %s %s\n", dest_port, ipv4_str, conf_file, rout_file);
#endif

  int payload_len = UDP_MAX_PAYLOAD_LENGTH;
  if (argc == 6) {
    char * payload_len_str = argv[5];
    payload_len = atoi(payload_len_str);
    if ((payload_len < 0) || (payload_len > UDP_MAX_PAYLOAD_LENGTH)) {
      fprintf(stderr, "%s: Longitud de payload incorrecta: \"%s\"\n",
              myself, payload_len_str);
      exit(-1);
    }
  }
  
  int er=udp_open(conf_file, rout_file, 2346);
  
  if(er==-1)
    exit(-1);
  
  /* Generar payload */
  unsigned char payload[payload_len];
  int i;
  for (i=0; i<payload_len; i++) {
    payload[i] = (unsigned char) i;
  }
  
#ifdef DEBUG
  printf("Sentencia udp_send\n");
#endif
  
  int e=0;
  e=udp_send ( IP_dest, dest_port, payload, payload_len);
  
  if(e==-1)
  {
    printf("Error\n");
    exit(-1);
  }
  if(e==0)
  {
    exit(0);
  }
#ifdef DEBUG
    printf("No Error\n");
#endif
  
  /**So we start to receive **/
  
  ipv4_addr_t src;
  uint16_t src_port;
  unsigned char data[UDP_MAX_PAYLOAD_LENGTH];
  long int timeout=2000;
  
  int error= udp_recv  (src, &src_port, data,  timeout );
  
  if(error==-1)
  {
    printf("Error Receiving data\n");
    exit(-1);
  }
  
  if(error==0)
  {
    printf("Timer expired, no packet received\n");
    exit(0);
  }
  
  print_pkt ( data, error, 0 );
 
  return 0;
}
