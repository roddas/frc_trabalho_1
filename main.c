#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<unistd.h>
#include<arpa/inet.h>

#define MAX_BUF 2014
#define DNS_PORT 53
#define NS 2

// DNS header structure, according to RFC-2909
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

int main(int argc, char ** argv)
{
	int udp_socket,return_status;
	socklen_t addr_len;
	struct sockaddr_in udp_client, udp_server;
	char buffer[MAX_BUF];
	
	if(argc < 2){
		fprintf(stderr,"Usage : %s <ip_address>\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	/* create a socket */
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_socket == -1){
		fprintf(stderr,"Could not create a socket!\n");
		exit(EXIT_FAILURE);
	}
	printf("Socket created. :-D\n");
	
	
	/* client address
	 * use INADDR_ANY  to use all local addresses */
	 udp_client.sin_family = AF_INET;
	 udp_client.sin_addr.s_addr = INADDR_ANY;
	 udp_client.sin_port = 0;
	 
	 return_status = bind(udp_socket,(struct sockaddr*)&udp_client, sizeof(udp_client));
	 if(return_status != 0){
		fprintf(stderr,"Could not bind address!.\n");
		//close(udp_socket);
		exit(EXIT_FAILURE);
	 }
	printf("Bind completed !\n");
	
	/* set up the message to be sent to the server */
	strcpy(buffer,"N");
	
	/* server addresss */
	udp_server.sin_family = AF_INET;
	udp_server.sin_addr.s_addr = inet_addr(argv[1]);
	udp_server.sin_port = htons(DNS_PORT);
	
	return_status = sendto(udp_socket,buffer, strlen(buffer)+1,0,(struct sockaddr*)&udp_server,sizeof(udp_server));
	
	if(return_status == -1){
		fprintf(stderr,"Could not send the message.\n");
	}else{
		puts("Message sent :-D");
	}
	
	/* message sent: look for confirmation */
	addr_len = sizeof(udp_server);
	return_status = recvfrom(udp_socket, buffer,MAX_BUF,0,(struct sockaddr*)&udp_server, &addr_len);
	if(return_status == -1){
		fprintf(stderr,"The server did not receive confirmation!\n");
	}else{
		buffer[return_status] = 0;
		printf("Received : %s \n",buffer);
	}
	
	close(udp_socket);
	return EXIT_SUCCESS;
}
