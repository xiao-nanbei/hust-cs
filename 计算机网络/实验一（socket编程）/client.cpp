#include <iostream>
#include "chattingroom.h"
#include "filetra.h"
#include "html.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;
int main() {
    //ѡ����
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
    //���峤�ȱ���

    //���巢�ͻ������ͽ��ܻ�����


    return 0;

}
