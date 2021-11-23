//
// Created by databrains on 2021/11/2.
//
#include "RdtSender.h"
#ifndef STOPWAIT_SRRDTSENDER_H
#define STOPWAIT_SRRDTSENDER_H
#define len 8
class SRRdtSender :public RdtSender{
private:
    int base;
    int nextseqnum;
    Packet win[len];
    bool win_flag_ack[len]={false};
    bool waitingState;//是否处于等待ACK状态
    int num_packet_win;
public:
    bool getWaitingState();
    bool send(const Message &message);
    void receive(const Packet & ackPkt);
    void timeoutHandler(int seqNum);
public:
    SRRdtSender();
    virtual ~SRRdtSender();
};
#endif //STOPWAIT_SRRDTSENDER_H
