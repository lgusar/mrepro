#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define HOST 0
#define NETWORK 1
#define DEC 0
#define HEX 1


void usage(){
    printf("prog[-r][-t|-u][-x][-h|-n][-46]{hostname|IP_address}{servicename|port}\n");
    exit(1);
}

int main(int argc, char **argv){
    
    // 2 or more arguments are required
    if (argc < 3) usage();

    char *hostname;
    char *servicename;

    char addrstr[100];

    int gai_error;

    char ch;

    uint16_t port;
    
    //specifying which protocol to use (TCP or UDP)
    int protocol = -1;

    //viewing port number in DEC or HEX
    int view = -1;

    // host byte order or network byte order
    int endian = -1;

    // ipv4 or ipv6
    int ipv = -1;

    //switch the program to using getnameinfo
    int reverse_lookup = 0;
    
    while ((ch = getopt(argc, argv, "rtuxhn46")) != -1){
        switch(ch){
            case 'r':
                reverse_lookup = 1;
                break;
            case 't':
                if (protocol == IPPROTO_UDP){
                    fprintf(stderr, "already defined udp protocol\n");
                    usage();
                }
                protocol = IPPROTO_TCP;
                break;
           case 'u':
                if (protocol == IPPROTO_TCP){
                    fprintf(stderr, "already defined tcp protocol\n");
                    usage();
              }
                protocol = IPPROTO_UDP;
                break;
          case 'x':
                view = HEX;
                break;
          case 'h':
                if (endian == NETWORK){
                    fprintf(stderr, "already defined network byte order\n");
                    usage();
                }
                endian = HOST;
                break;
          case 'n':
                if (endian == HOST){
                    fprintf(stderr, "already defined host byte order\n");
                    usage();
                }
                endian = NETWORK;
                break;
          case '4':
                if (ipv == AF_INET6){
                    fprintf(stderr, "already defined ipv6\n");
                    usage();
                }
                ipv = AF_INET;
                break;
          case '6':
                if (ipv == AF_INET){
                    fprintf(stderr, "already defined ipv4\n");
                    usage();
                }
                ipv = AF_INET6;
                break;
          default:
                printf("unknown option -%c\n", ch);
        }
    }

    // setting the default values

    if (protocol == -1) protocol = IPPROTO_TCP;

    if (view == -1) view = DEC;

    if (endian == -1) endian = HOST;

    if (ipv == -1 ) ipv = AF_INET;
    
    // getaddrinfo is called
    if (!reverse_lookup){
        
        struct addrinfo hints;
        struct addrinfo *results;
        struct addrinfo *record;
   
        memset(&hints, 0, sizeof(hints));
    
        hints.ai_family = ipv;
        hints.ai_protocol = protocol;
        hints.ai_flags |= AI_CANONNAME;

        hostname = argv[argc - 2];
        servicename = argv[argc - 1];
 

        if ((gai_error = (getaddrinfo(hostname, servicename, &hints, &results))) != 0){
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_error));
            exit(EXIT_FAILURE);
        }

        for (record = results; record != NULL; record = record->ai_next){
            inet_ntop(record->ai_family,
                      &((struct sockaddr_in *) record->ai_addr)->sin_addr,
                      addrstr, 100);
            port = ((struct sockaddr_in *) record->ai_addr)->sin_port;
            
            if (endian == HOST){
                port = ntohs(port);
            }
            if (view == DEC){
                printf("%s (%s) %d\n", addrstr, record->ai_canonname, port);
            }
            else{
                printf("%s (%s) %.4x\n", addrstr, record->ai_canonname, port);
            }
        }
        freeaddrinfo(results);
    }
    

    //getnameinfo is called
    else{
        
        //work in progress

        char *ip_address;
        char *port_number;

        char host[100];
        char serv[100];

        struct sockaddr_in6 sa6;
        struct sockaddr_in sa;
        
        int gni_error;

        ip_address = argv[argc - 2];
        port_number = argv[argc - 1];

        port = atoi(port_number);
    
        if(ipv == AF_INET6){
            if (inet_pton(AF_INET6, ip_address, &(sa6.sin6_addr)) != 1){
                fprintf(stderr, "%s is not a valid IPv6 address\n", ip_address);
                usage();
            }
            sa6.sin6_port = htons(port);
            sa6.sin6_family = AF_INET6;
    	    if ((gni_error = (getnameinfo((struct sockaddr *)&sa6,
                sizeof(struct sockaddr_in6),
                host, 100,
                serv, 100,
                NI_NAMEREQD))) != 0){
                    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(gni_error));
                    usage();
                }
        }
        else{
            if (inet_pton(AF_INET, ip_address, &(sa.sin_addr)) != 1){
                fprintf(stderr, "%s is not a valid IPv4 address\n", ip_address);
                usage();
            }
            sa.sin_port = htons(port);
            sa.sin_family = AF_INET;
            if ((gni_error = (getnameinfo((struct sockaddr *)&sa,
                sizeof(struct sockaddr_in),
                host, 100,
                serv, 100,
                NI_NAMEREQD))) != 0){
                    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(gni_error));
                    usage();
            }
        }

        printf("%s (%s) %s\n", ip_address, host, serv);
        return 0;
    }
}
