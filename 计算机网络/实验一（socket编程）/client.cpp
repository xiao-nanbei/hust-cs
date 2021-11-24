#include <iostream>
#include "chattingroom.h"
#include "filetra.h"
#include "html.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;
int main() {
    //选择功能
    char cond;
    cout << "please select:"<<endl;
    cout << "1.chatting room   2.file translate"
                "3.html translate" << endl;
    scanf("%c",&cond);
    switch (cond) {
        case '1':cr();break;
        case '2':ft();break;
        case '3':htmltra();break;
        default:printf("error\n"); break;
    }
    //定义长度变量

    //定义发送缓冲区和接受缓冲区


    return 0;

}
