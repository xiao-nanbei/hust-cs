#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "GBNRdtReceiver.h"
#include "GBNRdtSender.h"
#include <iostream>
int main(int argc, char* argv[])
{
    RdtSender * ps = new GBNRdtSender();
    RdtReceiver * pr = new GBNRdtReceiver();
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
