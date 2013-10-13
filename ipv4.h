#ifndef _IPv4_H
#define _IPv4_H

#include <stdint.h>
#include <netinet/in.h>
#include "ipv4_config.h"
#include "ipv4_route_table.h"

#define IPv4_ADDR_SIZE 4
#define IPv4_STR_MAX_LENGTH 16
#define IPv4_HEADER_LENGTH 20
#define IP_MAX_PACKET_SIZE 1480
#define IPv4_MAX_PAYLOAD_LENGTH 1480
#define IP_PROT_TYPE 0x0800
#define IP_VERSION 4
#define IP_ETH_TYPE 0x0800


typedef unsigned char ipv4_addr_t [IPv4_ADDR_SIZE];
/* ipv4_open (char *config_file, char *route_table);
 *
 * DESCRIPCIÓN: 
 *   Esta función inicializa la interfaz Ethernet especificada en los archivos
 *   de configuración llamando a eth_open(), inicializa la tabla de rutas, y 
 *   obtiene la IP propia a través de los archivos de configuración.
 *
 *   La memoria del manejador de interfaz devuelto debe ser liberada con la
 *   función 'ipv4_close()'.
 *
 * PARÁMETROS:
 *   'config_file': Nombre del fichero de configuración que se desea leer.
 *
 *   'route_table': Tabla de rutas.
 *
 * VALOR DEVUELTO:
 *   Ninguno
 *
 * ERRORES:
 *   La función devuelve 'NULL' si se ha producido algún error.
 */
 void ipv4_open(char *config_file, char *route_table);
/* int ipv4_close ();
 * 
 * DESCRIPCIÓN:
 *   Esta función cierra la interfaz Ethernet y libera la memoria
 *   de su manejador. (Borra tabla de rutas)
 *
 * PARÁMETROS:
 *   Ninguno.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si la interfaz Ethernet se ha cerrado correctamente.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
 void ipv4_close();
/* int ipv4_send 
 * (ipv4_addr_t dst, uint16_t type, unsigned char * payload, int payload_len);
 *
 * DESCRIPCIÓN:
 *   Esta función permite enviar un paquete IPv4 a la dirección IPv4
 *   indicada.
 *
 * PARÁMETROS:
 *         'dst': Dirección IP del equipo destino.
 *        'type': Valor del campo 'Tipo' de la trama Ethernet a enviar.
 *     'payload': Puntero a los datos que se quieren enviar en la trama
 *                Ethernet.
 * 'payload_len': Longitud en bytes de los datos a enviar.
 *
 * VALOR DEVUELTO:
 *   El número de bytes de datos que han podido ser enviados.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
 int ipv4_send( ipv4_addr_t dst, uint16_t type, unsigned char * payload, int payload_len);
/* int ipv4_receive 
 * (ipv4_addr_t src, uint16_t type, unsigned char buffer[], long int timeout);
 *
 * DESCRIPCIÓN:
 *   Esta función permite obtener el siguiente paquete IPv4 recibido por la
 *   interfaz Ethernet indicada. La operación puede esperar indefinidamente o
 *   un tiempo limitando dependiento del parámetro 'timeout'.
 *
 * PARÁMETROS:
 *      'src': Dirección IP del equipo origen.
 *     'type': Valor del campo 'Tipo' de la trama Ethernet que se desea
 *             recibir. 
 *   'buffer': Array donde se almacenarán los datos de la trama recibida.
 *             Deben reservarse al menos 'IPv4_IP_MAX_LENGTH_PAYLOADs' bytes para recibir dichos
 *             datos.
 *  'timeout': Tiempo en milisegundos que debe esperarse a recibir una trama
 *             antes de retornar. Un número negativo indicará que debe
 *             esperarse indefinidamente, mientras que con un '0' la función
 *             retornará inmediatamente, se haya recibido o no una trama.
 *
 * VALOR DEVUELTO:
 *   La longitud en bytes de los datos de la trama recibida, o '0' si no se ha
 *   recibido ninguna trama porque ha expirado el temporizador.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
 int ipv4_receive( ipv4_addr_t src, uint16_t type, unsigned char buffer[], long int timeout);
/*
 * uint16_t ipv4_checksum ( unsigned char * data, int len )
 *
 * DESCRIPCIÓN:
 *   Esta función calcula el checksum IP de los datos especificados.
 *
 * PARÁMETROS:
 *   'data': Puntero a los datos sobre los que se calcula el checksum.
 *    'len': Longitud en bytes de los datos.
 *
 * VALOR DEVUELTO:
 *   El valor del checksum calculado.
 */
 uint16_t ipv4_checksum ( unsigned char * data, int len );


/* void ipv4_addr_str ( ipv4_addr_t addr, char* str );
 *
 * DESCRIPCIÓN:
 *   Esta función genera una cadena de texto que representa la dirección IPv4
 *   indicada.
 *
 * PARÁMETROS:
 *   'addr': La dirección IP que se quiere representar textualente.
 *    'str': Memoria donde se desea almacenar la cadena de texto generada.
 *           Deben reservarse al menos 'IPv4_STR_MAX_LENGTH' bytes.
 */
 void ipv4_addr_str ( ipv4_addr_t addr, char* str );
/* int ipv4_str_addr ( char* str, ipv4_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función analiza una cadena de texto en busca de una dirección IPv4.
 *
 * PARÁMETROS:
 *    'str': La cadena de texto que se desea procesar.
 *   'addr': Memoria donde se almacena la dirección IPv4 encontrada.
 *
 * VALOR DEVUELTO:
 *   Se devuelve 0 si la cadena de texto representaba una dirección IPv4.
 *
 * ERRORES:
 *   La función devuelve -1 si la cadena de texto no representaba una
 *   dirección IPv4.
 */
 int ipv4_str_addr ( char* str, ipv4_addr_t addr );


#endif /* _IPv4_H */
