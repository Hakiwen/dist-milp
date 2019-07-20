#include "udp_driver.h"

int UDPSocket::init(std::string dIP, std::string sIP, uint16_t dPort, uint16_t sPort) {
    
    const char *cdIP = dIP.data();
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(cdIP);
    addr.sin_port = htons(dPort);
    int addrlen = sizeof(addr);

    const char *csIP = sIP.data();
    memset(&srcaddr, 0, sizeof(srcaddr));
    srcaddr.sin_family = AF_INET;
    srcaddr.sin_addr.s_addr = inet_addr(csIP);
    srcaddr.sin_port = htons(sPort);
    int srcaddrlen = sizeof(srcaddr);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    if (bind(fd, (struct sockaddr *) &srcaddr, sizeof(srcaddr)) < 0) {
        perror("bind");
        exit(1);
    }
    initYet = 1;
    return 0;
}

int UDPSocket::sendPacket(uint8_t *msgToSend, uint32_t msgSize) {
    if (sendto(fd, msgToSend, msgSize, 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("sendto");
        exit(1);
    }
    return 0;
}

int UDPSocket::receivePacket(uint8_t *data, uint32_t maxMsgSize) {
    int srcaddrlen = sizeof(srcaddr);
    int msgSize = recvfrom(fd, data, maxMsgSize, 0, (struct sockaddr *)&srcaddr, (socklen_t*) srcaddrlen);
    return 0;
}















