#define PORT_LEN 22
#define ADDR_LEN 16
#define MAX_ENTRIES

struct pairs
{
    char ip_address[ADDR_LEN];
    char port_number[PORT_LEN];
};


struct msg
{
    char command;
    int number_of_pairs;
    struct pairs entry[MAX_ENTRIES];

};
