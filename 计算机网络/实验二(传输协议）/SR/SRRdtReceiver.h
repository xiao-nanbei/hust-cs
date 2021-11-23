//
// Created by databrains on 2021/11/2.
//
#include "RdtReceiver.h"
#ifndef STOPWAIT_SRRDTRECEIVER_H
#define STOPWAIT_SRRDTRECEIVER_H
#include "queue"
#define len 8
class SRRdtReceiver :public RdtReceiver{
private:
    int rcv_base;
    std::deque<Packet> cache;
    Packet lastAckPkt;
public:
    SRRdtReceiver();
    virtual ~SRRdtReceiver();

public:
    void receive(const Packet &packet);
};
#endif //STOPWAIT_SRRDTRECEIVER_H
