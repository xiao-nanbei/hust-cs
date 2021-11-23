#include "Global.h"
#include "TCPRdtSender.h"
TCPRdtSender::TCPRdtSender():base(1),nextseqnum(1),waitingState(false),num_pac_win(0){}

TCPRdtSender::~TCPRdtSender(){}

bool TCPRdtSender::getWaitingState()
{
    return waitingState;
}


bool TCPRdtSender::send(const struct Message & message)
{
    if(nextseqnum < base +len)//窗口未满时
    {
        waitingState = false;//窗口不满
        win[num_pac_win].acknum = -1;
        win[num_pac_win].seqnum = nextseqnum;//初始值为1
        win[num_pac_win].checksum = 0;
        memcpy(win[num_pac_win].payload, message.data, sizeof(message.data));//获取应用层数据
        win[num_pac_win].checksum = pUtils->calculateCheckSum(win[num_pac_win]);//计算检查和

        pUtils->printPacket("发送方发送报文",win[num_pac_win]);
        if(base == nextseqnum)//如果是base的报文，则启动定时器
            pns->startTimer(SENDER, Configuration::TIME_OUT, win[num_pac_win].seqnum);
        pns->sendToNetworkLayer(RECEIVER, win[num_pac_win]);//发送报文至网络层
        num_pac_win++;//窗口内packet数目+1

        if(num_pac_win > len)//如果满了，则更改状态
            waitingState = true;

        nextseqnum++;//发送下一个报文nextsequm+1；
        return true;
    }

    else//窗口已经满时
    {
        waitingState = true;
        return false;
    }
}

void TCPRdtSender::receive(const struct Packet & ackPkt)
{
    if(num_pac_win > 0 )//如果窗口内有待确认的报文
    {
        int checkSum = pUtils->calculateCheckSum(ackPkt);//计算检查和

        printf("receive_ACK number：%d\n", ackPkt.acknum);
        printf("base number： %d\n",base);

        if(checkSum == ackPkt.checksum && ackPkt.acknum >= base)//如果ack_num比base更大
        {

            if(ackPkt.acknum == this ->win[0].seqnum)//判断现在的序号和上一次的序号是否相同
            {
                count++;//计数+1
                if(count == 4)//如果ack四次，则重传当前窗口的第一个报文
                {
                    printf("快速重传！！！");
                    pns->stopTimer(SENDER,win[0].seqnum);
                    pns->sendToNetworkLayer(RECEIVER,win[0]);//将第一个报文发送给接收方
                    pUtils->printPacket("\n冗余ACK*4，快速重传当前窗口第一个报文",win[0]);
                    pns->startTimer(SENDER,Configuration::TIME_OUT,win[0].seqnum);
                    printf("\n冗余ACK%d *4 \n",ackPkt.acknum);
                    count = 0;
                    return;
                }//冗余ack快速重传
            }

            else {
                count = 1;
            }

            if(this -> count != 1)
                return;

            else
            {
                int num = ackPkt.acknum - base ;
                base = ackPkt.acknum;//发送方的ack为期待收到的base报文的序号
                pUtils->printPacket("接收到ACK报文", ackPkt);

                if(base == nextseqnum)//如果收到的确认是发送窗口中的最后一个报文的确认
                    pns->stopTimer(SENDER, win[0].seqnum);
                else
                {
                    pns->stopTimer(SENDER, win[0].seqnum);
                    pns->startTimer(SENDER,Configuration::TIME_OUT,win[num].seqnum);//启动新的计时器
                }
                for(int i = 0; i < num_pac_win - num ; i++)
                {
                    win[i] = win[i+num];
                    printf("The current windows's %d number is %d\n",i,win[i].seqnum);
                }
                num_pac_win = num_pac_win - num;//平移窗口
            }}}}


void TCPRdtSender::timeoutHandler(int seqNum)
{
    pUtils->printPacket("发送方定时器超时，重新发送报文段",win[0]);
    pns->stopTimer(SENDER,win[0].seqnum);
    pns->startTimer(SENDER, Configuration::TIME_OUT,win[0].seqnum);
    pns->sendToNetworkLayer(RECEIVER, win[0]);

}
