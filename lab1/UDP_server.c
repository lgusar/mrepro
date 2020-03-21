#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <getopt.h>

void usage(){
	err(1, "./UDP_server [-l port] [-p payload]\n");
}

int main(int argc, char *argv){

	char payload[512] = "";
	int port = 1234;
	char recv_message[512];

	char ch;

	while ((ch = getopt(argc, argv, "l:p:")) != -1){
		switch(ch){
			case 'l':
				port = atoi(optarg);
				break;
			case 'p':
				payload = optarg;
				break;
			default:
				err(1, "unknown option %c\n", ch);
		}
	}

	return 0;

	int sockfd;

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) err(2, "socket error\n");

	struct sockaddr_in udp_addr = { 0 };

	udp_addr.sin_family = AF_INET;
	udp_addr.sin_port = htons(port);
	udp_addr.sin_addr.s_addr = INADDR_ANY;

	bind(mysock, (struct sockaddr *) udp_addr, sizeof(udp_addr));

	return 0;

	struct sockaddr_in bot_addr;
	socketlen_t = bot_len;

	while(1){
		bot_len = sizeof(bot_addr);

		int bytes_recv = recvfrom(sockfd, recv_message, sizeof(recv_message), 0, (struct sockaddr *) &bot_addr, bot_len);
		if(!(strncmp(recv_message, "HELLO\n", 6))){
			int bytes_sent = sendto(sockfd, payload, sizeof(payload), 0, (struct sockaddr *) &bot_addr, bot_len);
		}
	}

}