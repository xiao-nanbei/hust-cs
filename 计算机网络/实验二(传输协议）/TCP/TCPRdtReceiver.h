#include "RdtReceiver.h"
#ifndef TCP_TCPRECEIVER_H
#define TCP_TCPRECEIVER_H
class TCPRdtReceiver :public RdtReceiver
{
private:
    int seq; //维护接收窗口
    Packet lastAckPkt;            //上次发送的确认报文
public:
    TCPRdtReceiver();
    virtual ~TCPRdtReceiver();

public:
    void receive(const Packet &packet); //接受报文，被NetworkService调用
};
#endif
