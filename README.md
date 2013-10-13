Advanced networks and communication services
============================================

# RySCA project @ UC3M (2013/14)

librawnet is needed

rawnetcc arp_client arp_client.c arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c

/arp_client eth0 <IP_target>


rawnetcc ipv4_client ipv4_client.c arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c

rawnetcc ipv4_server ipv4_server.c arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c


- ipv4_config_server/client.txt

Interface eth0

IPv4Address 163.117.144.XXX

SubnetMask 255.255.255.0


- ipv4_route_table_client/server.txt

 SubnetAddr    SubnetMask      Iface    Gateway 

0.0.0.0         0.0.0.0         eth0     163.117.144.2

163.117.144.0    255.255.255.0   eth0     0.0.0.0


./ipv4_client 0x0800 <IP_server> <size_payload>

./ipv4_server 0x0800
