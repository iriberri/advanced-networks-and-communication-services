Advanced networks and communication services 
============================================

Librawnet is needed. Librawnet enables developing network protocols bypassing the Linux kernel 
stack. It access directly to the host interfaces and allows sending and
receiving "raw" packets by means of a Packet Socket.

* ETH Layer

> rawnetcc /tmp/eth_client eth_client.c eth.c 
> rawnetcc /tmp/eth_server eth_server.c eth.c 

> /tmp/eth_server eth0 0x1234
> /tmp/eth_client eth0 0x1234 00:22:4d:48:07:cf 2


* ARP Client

> rawnetcc arp_client arp_client.c arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c

> /arp_client eth0 <IP_target>


* IPv4 Client & Server

> rawnetcc ipv4_client ipv4_client.c arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c
> rawnetcc ipv4_server ipv4_server.c arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c

> /tmp/ipv4_client <prot> <IP_server> <size_payload>
> /tmp/ipv4_server <prot>

* UDP Layer

> rawnetcc /tmp/udp_client udp_client.c arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c udp.c
> /tmp/udp_client 163.117.144.101 2103 2103 700

> rawnetcc /tmp/udp_server udp_server.c arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c udp.c
> /tmp/udp_server 2103

* RIPv2 Daemon (and client)

> rawnetcc /tmp/rip_server rip2_server.c arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c udp.c ripv2_route_table.c rip.c
> rawnetcc /tmp/rip_client rip2_client.c arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c udp.c ripv2_route_table.c rip.c

> /tmp/rip_server
> /tmp/rip_client <server_ip_addr>
