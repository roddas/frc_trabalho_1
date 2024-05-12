#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<time.h>
#include<netdb.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<resolv.h>

#define MAX_BUF 65535
#define DNS_PORT 53
#define NS 2
#define MAX_IP_LEN INET_ADDRSTRLEN

typedef struct {
	
	unsigned short id; 
	uint8_t rd : 1;
	uint8_t tc;
	uint8_t aa;
	uint8_t opcode;
	uint8_t qr;
	uint8_t rcode;
	uint8_t cd;
	uint8_t ad;
	uint8_t z;
	uint8_t ra;
	
	unsigned short query_count;
	unsigned short ans_count;
	unsigned short auth_count;
	unsigned short add_count;
	
}dns_header;

typedef struct
{
	unsigned short qtype;
	unsigned short qclass;
}question;

typedef struct
{
	unsigned short type;
	unsigned short _class;
	unsigned int ttl;
	unsigned short data_len;
}r_data;

typedef struct
{
	unsigned char *name;
	r_data *resource;
	unsigned char *rdata;
} res_record;

typedef struct
{
	unsigned char *name;
	unsigned short qtype;
	unsigned short qclass;
} dns_query;

typedef struct sockaddr_in sockaddr_in;

unsigned char* resolve_hostname(char* hostname) {
    struct sockaddr_in addr;
    int ret;

    // Clear the address structure
    memset(&addr, 0, sizeof(addr));

    // Resolve the hostname
    ret = res_query(hostname, C_IN, T_A, (unsigned char *)&addr, sizeof(addr));
    if (ret < 0) {
        fprintf(stderr, "res_query: failed to resolve hostname\n");
        return NULL;
    }

    // Convert the address to a string
    char* ip = malloc(MAX_IP_LEN);
    if (ip == NULL) {
        perror("malloc");
        return NULL;
    }
    inet_ntop(AF_INET, &(addr.sin_addr), ip, MAX_IP_LEN);

    return (unsigned char *)ip;
}

dns_query get_query(char *domain){
	dns_query query;
	query.name = resolve_hostname(domain);
	query.qclass = htons(1);
	query.qtype = htons(1);
	
	return query;
}

dns_header get_header(void){
	dns_header header;
	srand(time(NULL));
	header.id = (unsigned short)htons(rand());
	header.qr = 0; //This is a query
	header.opcode = 0; //This is a standard query
	header.aa = 0; //Not Authoritative
	header.tc = 0; //This message is not truncated
	header.rd = 1; //Recursion Desired
	header.ra = 0; //Recursion not available! hey we dont have it (lol)
	header.z = 0;
	header.ad = 0;
	header.cd = 0;
	header.rcode = 0;
	header.query_count = htons(1); //we have only 1 question
	header.ans_count = 0;
	header.auth_count = 0;
	header.add_count = 0;
	
	return header;
}

void send_message(char *domain, char *server){
	
	/* set up the sockets */
	sockaddr_in udp_server;
	char buffer[MAX_BUF];
	
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	udp_server.sin_family = AF_INET;
	udp_server.sin_port = htons(DNS_PORT);
	udp_server.sin_addr.s_addr = inet_addr(server);
	
	/* set up the header */
	dns_header header = get_header();
	
	/* set up the query */
	dns_query query = get_query(domain);
	
	 // Copy header into buffer
    memcpy(buffer, &header, sizeof(dns_header));
    
    // Copy query into buffer after header
    memcpy(buffer + sizeof(dns_header), &query, sizeof(dns_query));
    
    int buffer_size = sizeof(dns_header) + sizeof(dns_query);
    
    int return_status = sendto(udp_socket, buffer, buffer_size, 0, (struct sockaddr*)&udp_server, sizeof(udp_server));
    
	if(return_status == -1){
		fprintf(stderr,"Could not send the message.\n");
	}else{
		puts("Message sent :-D");
		printf("Response status : %d\n",return_status);
	}
}

int main(int argc, char ** argv)
{
	
	if(argc < 3){
		fprintf(stderr,"Usage : %s  <dns_server> <fdnq>\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	send_message(argv[1],argv[2]);
	
	return EXIT_SUCCESS;
}
