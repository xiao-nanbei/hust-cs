#include "Global.h"
#include "GBNRdtSender.h"
GBNRdtSender::GBNRdtSender():base(1),nextseqnum(1),waitingState(false),num_packet_win(0)
{
}
GBNRdtSender::~GBNRdtSender()
{
}
bool GBNRdtSender::getWaitingState()
{
    return waitingState;
}
bool GBNRdtSender::send(const struct Message & message)
{
    //如果在窗口内，就继续发送
    if(nextseqnum < base + len)
    {
        //不在等待状态
        waitingState = false;
        //窗口的最后一个包+1设置为acknum=-1
        win[num_packet_win].acknum = -1;
        //窗口的最后一个包+1的seqnum设置为传回的nextseqnum
        win[num_packet_win].seqnum = nextseqnum;
        //校验和为0
        win[num_packet_win].checksum = 0;
        //将message的数据复制到payload,长度为data的长度
        memcpy(win[num_packet_win].payload, message.data, sizeof(message.data));
        //
        win[num_packet_win].checksum = pUtils->calculateCheckSum(win[num_packet_win]);
        pUtils->printPacket("Sender sends message",win[num_packet_win]);
        if(base == nextseqnum)
            pns->startTimer(SENDER, Configuration::TIME_OUT, win[num_packet_win].seqnum);

        pns->sendToNetworkLayer(RECEIVER, win[num_packet_win]);
        //窗口报文的数量++
        num_packet_win++;
        //如果大于窗口数组的长度
        if(num_packet_win > len)
            //开始等待
            waitingState = true;
        nextseqnum++;
        return true;
    }
    else
    {
        //开始等待
        waitingState = true;
        return false;
    }
}
void GBNRdtSender::receive(const struct Packet & ack)
{
    if(num_packet_win > 0 )
    {//如果窗口大于0
        //校验和
        int checkSum = pUtils->calculateCheckSum(ack);

        if(checkSum == ack.checksum && ack.acknum >= base)
        {
            int num = ack.acknum - base + 1;

            base = ack.acknum + 1;
            pUtils->printPacket("The sender received the correct confirmation", ack);

            if(base == nextseqnum)
                pns->stopTimer(SENDER, win[0].seqnum);
            else
            {
                pns->stopTimer(SENDER, win[0].seqnum);
                pns->startTimer(SENDER,Configuration::TIME_OUT,win[num].seqnum);
            }

            for(int i = 0; i < num_packet_win - num ; i++)
            {
                win[i] = win[i+num];
                printf("The current windows's %d number is %d\n",i,win[i].seqnum);
            }


            num_packet_win = num_packet_win - num;
        }
    }
}
void GBNRdtSender::timeoutHandler(int seqNum)
{
    pUtils->printPacket("The sender’s timer expires and retransmits the last message sent",win[0]);
    pns->stopTimer(SENDER,win[0].seqnum);
    pns->startTimer(SENDER, Configuration::TIME_OUT,win[0].seqnum);
    for(int i = 0; i < num_packet_win; i++)
    {
        pns->sendToNetworkLayer(RECEIVER, win[i]);
    }

}
