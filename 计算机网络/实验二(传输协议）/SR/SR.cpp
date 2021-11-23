#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "SRRdtReceiver.h"
#include "SRRdtSender.h"
int main(int argc, char* argv[])
{
    RdtSender * ps = new SRRdtSender();
    RdtReceiver * pr = new SRRdtReceiver();
    pns->setRunMode(1);
    pns->init();
    pns->setRtdSender(ps);
    pns->setRtdReceiver(pr);
    pns->setInputFile("/home/databrains/StopWait/bin/input.txt");
    pns->setOutputFile("/home/databrains/StopWait/bin/output.txt");
    pns->start();
    delete ps;
    delete pr;
    delete pUtils;
    delete pns;
    return 0;
}
