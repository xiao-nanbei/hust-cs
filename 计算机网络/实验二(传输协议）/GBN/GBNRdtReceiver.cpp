#include "Global.h"
#include "GBNRdtReceiver.h"
//构造函数
GBNRdtReceiver::GBNRdtReceiver():seq(1)
{
    //
    lastAckPkt.acknum = 0;
    lastAckPkt.checksum = 0;
    lastAckPkt.seqnum = -1;
    for(int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
        lastAckPkt.payload[i] = '.';
    lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}
GBNRdtReceiver::~GBNRdtReceiver()
{
}
void GBNRdtReceiver::receive(const struct Packet & packet)
{
    //校验和
    int checkSum = pUtils->calculateCheckSum(packet);
    //如果校验和等于包的校验和且seq序号吻合
    if(checkSum == packet.checksum &&seq == packet.seqnum)
    {
        //
        pUtils->printPacket("The receiver correctly receives the message from the sender",packet);
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER,msg);

        lastAckPkt.acknum = packet.seqnum;
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);

        pUtils->printPacket("Receiver sends confirmation message",lastAckPkt);
        pns->sendToNetworkLayer(SENDER,lastAckPkt);

       seq ++;
    }

    else
    {
        if(packet.acknum != seq)
            pUtils->printPacket("ERROR: The receiver did not receive the correct message: The message sequence number is wrong",packet);
        else
            pUtils->printPacket("ERROR: The receiver did not receive the correct message: checksum error",packet);

        pUtils->printPacket("ERROR: The receiver did not receive the correct message: checksum error",lastAckPkt);
        pns->sendToNetworkLayer(SENDER,lastAckPkt);

    }
}
