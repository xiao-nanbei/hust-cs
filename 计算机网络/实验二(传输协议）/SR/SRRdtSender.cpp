#include "Global.h"
#include "SRRdtSender.h"
SRRdtSender::SRRdtSender():base(0),nextseqnum(0),waitingState(false){}
SRRdtSender::~SRRdtSender(){}
bool SRRdtSender::getWaitingState() {
    return waitingState;
}

bool SRRdtSender::send(const struct Message & message){
    if(nextseqnum<=base+len){
        waitingState= false;
        win[nextseqnum-base].seqnum=nextseqnum;
        this->win[nextseqnum-base].acknum = -1;
        win_flag_ack[nextseqnum-base]= false;
        memcpy(win[nextseqnum-base].payload,message.data,sizeof(message.data));
        win[nextseqnum-base].checksum = pUtils->calculateCheckSum(win[nextseqnum-base]);
        pUtils->printPacket("Sender sends message",win[nextseqnum-base]);
        pns->startTimer(SENDER, Configuration::TIME_OUT, win[nextseqnum-base].seqnum);
        pns->sendToNetworkLayer(RECEIVER, win[nextseqnum-base]);
        nextseqnum++;
        if(nextseqnum-base>=len){
            waitingState= true;
        }
        return true;
    }
    else{
        waitingState=true;
        return false;
    }
}
void SRRdtSender::receive(const Packet &ackPkt) {
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    if(checkSum==ackPkt.checksum&&ackPkt.acknum>=base){
        pUtils->printPacket("The sender received the correct confirmation", ackPkt);
        win_flag_ack[ackPkt.acknum-base]=true;
        pns->stopTimer(SENDER,win[ackPkt.acknum-base].seqnum);
        int move_i=0;
        while(win_flag_ack[move_i]) {
            base++;
            move_i++;
            if(move_i==8){
                break;
            }
        }
        for(int i=0;i<len;i++){
            if(move_i+i>=len){
                Packet temp;
                temp.seqnum=base+i;
                win[i]=temp;
                win_flag_ack[i]= false;
            }else{
                win[i]=win[move_i+i];
                win_flag_ack[i]= win_flag_ack[move_i+i];
            }
            printf("The current windows's %d number is %d\n",i,win[i].seqnum);
        }
    }
}
void SRRdtSender::timeoutHandler(int seqNum)
{
    int i;
    for(i=0;i<nextseqnum-base;i++){
        if(win[i].seqnum==seqNum){
            break;
        }
    }
    pUtils->printPacket("The sender’s timer expires and retransmits",win[i]);
    pns->stopTimer(SENDER,win[i].seqnum);
    pns->startTimer(SENDER,Configuration::TIME_OUT, win[i].seqnum);
    pns->sendToNetworkLayer(RECEIVER,win[i]);

}
