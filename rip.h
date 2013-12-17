#define RIPV2_VERSION 2
#define RIP_REQUEST 1
#define RIP_RESPONSE 2
#define RIP_HEADER_SIZE 4
#define RIP_ENTRY_SIZE 20
#define RIP_PORT 520
#define INFINITE_TIMEOUT -1
#define RIP_TIMEOUT 180

/*http://www.tcpipguide.com/free/t_RIPVersion2RIP2MessageFormatandFeatures-3.htm */





typedef struct {
        uint16_t addr_family_id;
        uint16_t route_tag;
        ipv4_addr_t ip_addr;
        ipv4_addr_t mask;
        ipv4_addr_t next_hop;
        uint32_t metric;
} ripv2_entry_t;



 typedef struct {
        unsigned char command; //1Byte
        unsigned char version; // 1Byte
        uint16_t zero; //2Bytes
        ripv2_entry_t rip_entries[25];
} ripv2_message_t;



