#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

class UDPSocket
{
    struct sockaddr_in addr, srcaddr;
    int fd;
    public:
    int initYet = 0;
    int sendPacket(uint8_t *msgToSend, uint32_t msgSize);
    int receivePacket(uint8_t *data, uint32_t maxMsgSize);
    int init(std::string dIP, std::string sIP,uint16_t dPort, uint16_t sPort);
};
