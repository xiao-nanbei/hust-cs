#include "Global.h"
#include "SRRdtReceiver.h"
#include "stack"
SRRdtReceiver::SRRdtReceiver() :rcv_base(0){
    lastAckPkt.acknum = 0;
    lastAckPkt.seqnum = -1;
    for(int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
        lastAckPkt.payload[i] = '.';
    lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}
SRRdtReceiver::~SRRdtReceiver(){}
void SRRdtReceiver::receive(const Packet &packet) {
    int checkSum=pUtils->calculateCheckSum(packet);
    if(checkSum==packet.checksum){
        if(packet.seqnum>=rcv_base&&packet.seqnum<rcv_base+len){
            //收到的packet大于接收端基序号，小于窗口最大值
            pUtils->printPacket("The receiver correctly receives the message from the sender",packet);
            if(packet.seqnum==rcv_base){
                Message temp;
                memcpy(temp.data, packet.payload, sizeof(packet.payload));
                pns->delivertoAppLayer(RECEIVER,temp);
                rcv_base++;
                lastAckPkt.acknum=packet.seqnum;
                lastAckPkt.checksum=pUtils->calculateCheckSum(lastAckPkt);
                pUtils->printPacket("Receiver sends confirmation message",lastAckPkt);
                pns->sendToNetworkLayer(SENDER,lastAckPkt);
                while(!cache.empty()&&cache.front().seqnum==rcv_base){
                    Message temp_cache;
                    memcpy(temp_cache.data, cache.front().payload, sizeof(cache.front().payload));
                    pns->delivertoAppLayer(RECEIVER,temp_cache);
                    cache.pop_front();
                    rcv_base++;
                }
            }
            else if(packet.seqnum>rcv_base){
                if(cache.size()<7){
                    std::stack<Packet> temp;
                    while(!cache.empty()&&packet.seqnum>cache.front().seqnum){
                        temp.push(cache.front());
                        cache.pop_front();
                    }
                    if(packet.seqnum==cache.front().seqnum){
                        while (!temp.empty()){
                            cache.push_front(temp.top());
                            temp.pop();
                        }
                        lastAckPkt.acknum=packet.seqnum;
                        lastAckPkt.checksum=pUtils->calculateCheckSum(lastAckPkt);
                        pUtils->printPacket("Receiver sends confirmation message",lastAckPkt);
                        pns->sendToNetworkLayer(SENDER,lastAckPkt);
                        return;
                    }else{
                        cache.push_front(packet);
                        while (!temp.empty()){
                            cache.push_front(temp.top());
                            temp.pop();
                        }
                        lastAckPkt.acknum=packet.seqnum;
                        lastAckPkt.checksum=pUtils->calculateCheckSum(lastAckPkt);
                        pUtils->printPacket("Receiver sends confirmation message",lastAckPkt);
                        pns->sendToNetworkLayer(SENDER,lastAckPkt);
                    }
                }
            }
        }
        else if(packet.seqnum>=rcv_base-len&&packet.seqnum<rcv_base){
            lastAckPkt.acknum=packet.seqnum;
            lastAckPkt.checksum=pUtils->calculateCheckSum(lastAckPkt);
            pUtils->printPacket("Receiver sends confirmation message",lastAckPkt);
            pns->sendToNetworkLayer(SENDER,lastAckPkt);
        }
    }
}

