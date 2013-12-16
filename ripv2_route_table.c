#include "ipv4_route_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


 struct ripv2_route_table_t {
  ripv2_route_t * routes[RIPv2_ROUTE_TABLE_SIZE];
};


ripv2_route_t * ripv2_route_create
( ipv4_addr_t subnet, ipv4_addr_t mask, char* iface, ipv4_addr_t next_hop, int metric, timerms_t timer )
{
  ripv2_route_t * route = (ripv2_route_t *) malloc(sizeof(struct ripv2_route));

  if (route != NULL) {
    memcpy(route->subnet_addr, subnet, IPv4_ADDR_SIZE);
    memcpy(route->subnet_mask, mask, IPv4_ADDR_SIZE);
    memcpy(route->iface, iface, IFACE_NAME_LENGTH);
    memcpy(route->next_hop, next_hop, IPv4_ADDR_SIZE);
    route->metric = metric;
    route->timer = timer;
  }
  
  return route;
}



int ripv2_route_lookup ( ripv2_route_t * route, ipv4_addr_t addr )
{
  int prefix_length = -1;

/* IP for comparison */
 ipv4_addr_t aux;

/* Variable used to make the sum*/
 int prefix_length_aux =0;
 
 int i;

 /*Traverse each byte of the given IP addres and comparte
 it with the subnet mask of the given route */
for(i =0; i<IPv4_ADDR_SIZE; i++){
  /*Apply mask to IP address that we are checking */
  aux[i] = addr[i] & route->subnet_mask[i];
}

/* When mask is applied to IP address, this should be equal
to the route in order to send through this route
Example:
Route: 10.0.0.0 Mask 255.255.0.0
IP: 10.0.254.231
Apply mask to IP -> 10.0.0.0 = route
If
IP: 192.168.1.1
Apply mask to IP -> 192.168.0.0 != route
*/
  if(memcmp(aux, route->subnet_addr, IPv4_ADDR_SIZE)!=0){
    printf("Longitud de prefijo primero: %d", prefix_length);
    return prefix_length;
  }
int j;
/* Traverse the compared result to obtain the number of
bit that are equal, this is, the number of "ones"*/
 for(j =0; j<IPv4_ADDR_SIZE; j++){
 switch (route->subnet_mask[j]){
  case 255:
    prefix_length_aux+=8;
    break;
  case 254:
        prefix_length_aux+=7;
break;
  case 252:
      prefix_length_aux+=6;
break;
  case 248:
        prefix_length_aux+=5;
break;
  case 240:
        prefix_length_aux+=4;
break;
  case 224:   
        prefix_length_aux+=3;
break;
  case 192:
        prefix_length_aux+=2;
break;
  case 128:
        prefix_length_aux+=1;
break;
  case 0:
        prefix_length_aux+=0;
break;
}
}

/*As we return -1 if there is an error or the comparison 
is null, we check that there are coincidences in order to 
give the prefix_length value. We sum an extra 1 because of
the initial variable value was -1. */
  printf("Longitud de prefijo final: %d\n", prefix_length_aux);
  return prefix_length_aux;
}



void ipv4_route_print ( ripv2_route_t * route )
{
  if (route != NULL) {
    char subnet_str[IPv4_STR_MAX_LENGTH];
    ipv4_addr_str(route->subnet_addr, subnet_str);
    char mask_str[IPv4_STR_MAX_LENGTH];
    ipv4_addr_str(route->subnet_mask, mask_str);
    char* iface_str = route->iface;
    char next_hop_str[IPv4_STR_MAX_LENGTH];
    ipv4_addr_str(route->next_hop, next_hop_str);
    unsigned int metric = route->metric;

    printf("%s/%s -> %s/%s (%d)", subnet_str, mask_str, iface_str, next_hop_str, 
	   metric);
  }
}



void ripv2_route_free ( ripv2_route_t * route )
{
  if (route != NULL) {
    free(route);
  }
}



ripv2_route_table_t * ripv2_route_table_create()
{
  ripv2_route_table_t * table;

  table = (ripv2_route_table_t *) malloc(sizeof(struct ripv2_route_table));
  if (table != NULL) {
    int i;
    for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
      table->routes[i] = NULL;
    }
  }

  return table;
}

int ripv2_route_table_add ( ripv2_route_table_t * table, ripv2_route_t * route )
{
  int route_index = -1;

  if (table != NULL) {
    /* Find an empty place in the route table */
    int i;
    for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
      if (table->routes[i] == NULL) {
        table->routes[i] = route;
        route_index = i;
        break;
      }
    }
  }

  return route_index;
}



ripv2_route_t * ripv2_route_table_remove ( ripv2_route_table_t * table, int index )
{
  ripv2_route_t * removed_route = NULL;
  
  if ((table != NULL) && (index >= 0) && (index < RIPv2_ROUTE_TABLE_SIZE)) {
    removed_route = table->routes[index];
    table->routes[index] = NULL;
  }

  return removed_route;
}



ripv2_route_t * ripv2_route_table_get ( ripv2_route_table_t * table, int index )
{
  ripv2_route_t * route = NULL;

  if ((table != NULL) && (index >= 0) && (index < RIPv2_ROUTE_TABLE_SIZE)) {
    route = table->routes[index];
  }
  
  return route;
}



int ripv2_route_table_find
( ripv2_route_table_t * table, ipv4_addr_t subnet, ipv4_addr_t mask )
{
  int route_index = -2;

  if (table != NULL) {
    route_index = -1;
    int i;
    for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
      ripv2_route_t * route_i = table->routes[i];
      if (route_i != NULL) {
        int same_subnet = 
          (memcmp(route_i->subnet_addr, subnet, IPv4_ADDR_SIZE) == 0);
        int same_mask = 
          (memcmp(route_i->subnet_mask, mask, IPv4_ADDR_SIZE) == 0);
        
        if (same_subnet && same_mask) {
          route_index = i;
          break;
        }
      }
    }
  }

  return route_index;
}



ripv2_route_t * ripv2_route_table_lookup ( ripv2_route_table_t * table, 
                                         ripv2_addr_t addr )
{
  ripv2_route_t * best_route = NULL;
  int best_route_prefix = -1;

  if (table != NULL) {
    int i;
    for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
      ripv2_route_t * route_i = table->routes[i];
      if (route_i != NULL) {
        int route_i_lookup = ripv2_route_lookup(route_i, addr);
        if (route_i_lookup > best_route_prefix) {
          best_route = route_i;
          best_route_prefix = route_i_lookup;
        }
      }
    }
  }
  
  return best_route;
}


void ripv2_route_table_free ( ripv2_route_table_t * table )
{
  if (table != NULL) {
    int i;
    for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
      ipv4_route_t * route_i = table->routes[i];
      if (route_i != NULL) {
        table->routes[i] = NULL;
        ripv2_route_free(route_i);
      }
    }
    free(table);
  }
}


int ipv4_route_table_read ( char * filename, ipv4_route_table_t * table )
{
  int read_routes = 0;

  FILE * routes_file = fopen(filename, "r");
  if (routes_file == NULL) {
    fprintf(stderr, "Error opening input IPv4 Routes file \"%s\": %s.\n",
            filename, strerror(errno));
    return -1;
  }

  int linenum = 0;
  char line_buf[1024];
  char subnet_str[256];
  char mask_str[256];
  char iface_name[256];
  char next_hop_str[256];
  int err = 0;
  int metric;

    while ((!feof(routes_file)) && (err == 0)) {

                linenum++;

                /* Read next line of file */
                char* line = fgets(line_buf, 1024, routes_file);
                if (line == NULL) {
                        break;
                }

                /* If this line is empty or a comment, just ignore it */
                if ((line_buf[0] == '\n') || (line_buf[0] == '#')) {
                        err = 0;
                        continue;
                }

                /* Parse line: Format "<subnet> <mask> <nhop> <iface>\n" */
                err = sscanf(line, "%s %s %s %s %d\n", subnet_str, mask_str, iface_name, next_hop_str, &metric);
                if (err != 5) {
                        fprintf(stderr, "%s:%d: Invalid RIPv2 Route format: \"%s\" (%d items)\n", filename, linenum, line, err);
                        fprintf(stderr, "%s:%d: Format must be: <subnet> <mask> <iface> <next hop> <metric>\n", filename, linenum);
                        err = -1;

                } else {

                        /* Parse RIPv2 route subnet address */
                        ipv4_addr_t subnet;
                        err = ipv4_str_addr(subnet_str, subnet);
                        if (err == -1) {
                                fprintf(stderr, "%s:%d: Invalid <subnet> value: \"%s\"\n", filename, linenum, subnet_str);
                                break;
                        }

                        /* Parse RIPv2 route subnet mask */
                        ipv4_addr_t mask;
                        err = ipv4_str_addr(mask_str, mask);
                        if (err == -1) {
                                fprintf(stderr, "%s:%d: Invalid <mask> value: \"%s\"\n", filename, linenum, mask_str);
                                break;
                        }

                        /* Parse RIPv2 route next hop */
                        ipv4_addr_t next_hop;
                        err = ipv4_str_addr(next_hop_str, next_hop);
                        if (err == -1) {
                                fprintf(stderr, "%s:%d: Invalid <next hop> value: \"%s\"\n", filename, linenum, next_hop_str);
                                break;
                        }
                        timerms_t timer;
                        timerms_reset(&timer, INFINITE_TIMEOUT);
                        /* Create new route & add it to Route Table */
                        ripv2_route_t * new_route = ripv2_route_create(subnet, mask, iface_name, next_hop, metric, timer);

                        if (table != NULL) {
                                err = ripv2_route_table_add(table, new_route);
                                if (err >= 0) {
                                        err = 0;
                                        read_routes++;
                                }
                        }
                }
        } /* while() */

        if (err == -1) {
                read_routes = -1;
        }

        /* Close IP Configuration file */
        fclose(routes_file);

        return read_routes;
}


int ripv2_route_table_output ( FILE * out, ripv2_route_table_t * table)
{
         int err = fprintf(out, "# SubnetAddr  \tSubnetMask    \tIface  \tNextHop       \tMetric\tTimer\n");
        if (err < 0) {
                return -1;
        }

        char subnet_str[IPv4_ADDR_STR_LENGTH];
        char mask_str[IPv4_ADDR_STR_LENGTH];
        char* ifname = NULL;
        char nhop_str[IPv4_ADDR_STR_LENGTH];
        int metric;
        int i;
        for (i = 0; i < RIPv2_ROUTE_TABLE_SIZE; i++) {
                ripv2_route_t * route_i = ripv2_route_table_get(table, i);
                if (route_i != NULL) {
                        ipv4_addr_str(route_i->subnet_addr, subnet_str);
                        ipv4_addr_str(route_i->subnet_mask, mask_str);
                        ifname = route_i->iface;
                        ipv4_addr_str(route_i->next_hop_addr, nhop_str);
                        metric = route_i->metric;
                        long int remain = timerms_left(&(route_i->timer));
                        err = fprintf(out, "%-15s\t%-15s\t%s\t%s\t%d\t%ld\n", subnet_str, mask_str, ifname, nhop_str, metric, remain);
                        if (err < 0) {
                                return -1;
                        }
                }
        }

        return 0;
}



int ripv2_route_table_write ( ripv2_route_table_t * table, char * filename )
{
  int num_routes = 0;

  FILE * routes_file = fopen(filename, "w");
  if (routes_file == NULL) {
    fprintf(stderr, "Error opening output RIPv2 Routes file \"%s\": %s.\n",
            filename, strerror(errno));
    return -1;
  }

  fprintf(routes_file, "# %s\n", filename);
  fprintf(routes_file, "#\n");

  if (table != NULL) {
    num_routes = ripv2_route_table_output (routes_file, table, 0);
    if (num_routes == -1) {
      fprintf(stderr, "Error writing RIPv2 Routes file \"%s\": %s.\n",
              filename, strerror(errno));
      return -1;
    }
  }

  fclose(routes_file);
  
  return num_routes;
}



void ripv2_route_table_print ( ripv2_route_table_t * table )
{
  if (table != NULL) {
    ripv2_route_table_output (stdout, table);
  }
}
