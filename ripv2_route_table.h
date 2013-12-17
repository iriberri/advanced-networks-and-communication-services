#include "ipv4.h"
#include "ipv4_config.h"

#include <stdio.h>
#include <timerms.h>

#define ADMIN_DIST_DIRECT 0
#define ADMIN_DIST_STATIC 1
#define RIPv2_ROUTE_TABLE_SIZE 256
#define INFINITE_TIMEOUT -1

typedef struct ripv2_route{
  ipv4_addr_t subnet_addr;
  ipv4_addr_t subnet_mask;
  char iface[IFACE_NAME_LENGTH];
  ipv4_addr_t next_hop;
  int metric;
  timerms_t timer;
  
} ripv2_route_t;

 typedef struct ripv2_route_table_t {
  ripv2_route_t * routes[RIPv2_ROUTE_TABLE_SIZE];
} ripv2_route_table_t;

ripv2_route_t * ripv2_route_create(ipv4_addr_t subnet, ipv4_addr_t mask, char* iface, ipv4_addr_t nhop, int metric, long int timeout);


int ripv2_route_lookup ( ripv2_route_t * route, ipv4_addr_t addr );


void ripv2_route_print ( ripv2_route_t * route );


void ripv2_route_free ( ripv2_route_t * route );


#define IPv4_ROUTE_TABLE_SIZE 256



ripv2_route_table_t * ripv2_route_table_create();


int ripv2_route_table_add ( ripv2_route_table_t * table, ripv2_route_t * route );

ripv2_route_t * ripv2_route_table_remove ( ripv2_route_table_t * table, int index );


ripv2_route_t * ripv2_route_table_get ( ripv2_route_table_t * table, int index );


int ripv2_route_table_find
( ripv2_route_table_t * table, ipv4_addr_t subnet, ipv4_addr_t mask );


ripv2_route_t * ripv2_route_table_lookup ( ripv2_route_table_t * table, 
                                         ipv4_addr_t addr );

void ripv2_route_table_free ( ripv2_route_table_t * table );


int ripv2_route_table_read ( char * filename, ripv2_route_table_t * table );


int ripv2_route_table_write ( ripv2_route_table_t * table, char * filename );


void ripv2_route_table_print ( ripv2_route_table_t * table );

