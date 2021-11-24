#include <iostream>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;
int main() {
    //定义数据长度
    WSADATA wsadata;//初始化
    fd_set rfds;				//用于检查socket是否有数据到来的的文件描述符，用于socket非阻塞模式下等待网络事件通知（有数据到来）
    fd_set wfds;				//用于检查socket是否可以发送的文件描述符，用于socket非阻塞模式下等待网络事件通知（可以发送数据）
    bool first_connetion = true;
    
    int err = WSAStartup(MAKEWORD(2,2),&wsadata);//调用2.2版本的socket API
    if (err!=0){
        cout << "Failed to initialize socket library!" << endl;
    }
    else {
        cout << "Successfully initialized the socket library!" << endl;
    }
    //创建套接字
    SOCKET servSocket = socket(AF_INET,SOCK_STREAM,0);//流式套接字
    /* struct sockaddr和struct sockaddr_in这两个结构体用来处理网络通信的地址。
     * sockaddr的缺陷是：sa_data把目标地址和端口信息混在一起了
     * sockaddr_in解决了sockaddr的缺陷，把port和addr分开储存在两个变量中
     * */
    SOCKADDR_IN addr;
    addr.sin_family= AF_INET;//地址族,IPV4
    FILE *f=fopen(R"(C:\Users\34743\Desktop\tanxuan\xuantan_Server\server.txt)","rb");
    char buf[100];
    fgets(buf,sizeof(buf),f);
    u_short port;
    sscanf(buf,"%uh",&port);
    memset(buf,0,sizeof (buf));
    fgets(buf,sizeof(buf),f);
    u_long ADDR;
    sscanf(buf,"%x",&ADDR);
    memset(buf,0,sizeof (buf));
    addr.sin_port=htons(port);//16位TCP/UDP端口号
    addr.sin_addr.S_un.S_addr=htonl(ADDR);//INADDR_ANY本机的IP
    //bind函数将套接字与特定的IP地址和端口绑定起来
    if (bind(servSocket,(LPSOCKADDR)&addr,sizeof(addr))==SOCKET_ERROR){
        cout << "Socket binding failed!" << endl;
        WSACleanup();
    }
    else {
        cout << "The socket is bound successfully!" << endl;
    }
    //进入监听状态
    if (listen(servSocket,20)<0){
        cout << "Failed to set  status!" << endl;
        WSACleanup();
    }
    else {
        cout << "The  status is set successfully!" << endl;
    }
    u_long blockMode = 1;//将srvSock设为非阻塞模式以监听客户连接请求
    if ((ioctlsocket(servSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO：允许或禁止套接口s的非阻塞模式。
        cout << "ioctlsocket() failed with error!\n";
        return 0;
    }
    cout << "ioctlsocket() for session socket ok!	Waiting for client connection and data!\n";

    //清空read,write描述符，对rfds和wfds进行了初始化，必须用FD_ZERO先清空，下面才能FD_SET
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    //设置等待客户连接请求
    FD_SET(servSocket, &rfds);
    char rec[1024];

    while(true){
        SOCKET clntSock;
        //清空read,write描述符
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        //设置等待客户连接请求
        FD_SET(servSocket, &rfds);

        if (!first_connetion) {
            //设置等待会话SOKCET可接受数据或可发送数据
            FD_SET(clntSock, &rfds);
            FD_SET(clntSock, &wfds);
        }

        //开始等待
        int nTotal = select(0, &rfds, &wfds, nullptr, nullptr);

        //如果srvSock收到连接请求，接受客户连接请求
        if (FD_ISSET(servSocket, &rfds)) {
            nTotal--;

            //产生会话SOCKET
            int nSize=sizeof(SOCKADDR);
            SOCKADDR_IN clntAddr;
            clntSock = accept(servSocket, (LPSOCKADDR)&clntAddr, &nSize);

            if (clntSock != INVALID_SOCKET) {
                printf("Socket listen one client request!\n");
                cout << "the addr is:" << endl;
                cout << inet_ntoa(clntAddr.sin_addr) << endl;
                cout << "the port is:" << endl;
                cout << clntAddr.sin_port << endl;
            }
            //把会话SOCKET设为非阻塞模式
            if ((ioctlsocket(clntSock, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO：允许或禁止套接口s的非阻塞模式。
                cout << "ioctlsocket() failed with error\n";
                return 0;
            }
            //设置等待会话SOKCET可接受数据或可发送数据
            FD_SET(clntSock, &rfds);
            FD_SET(clntSock, &wfds);

            first_connetion = false;

        }

        //检查会话SOCKET是否有数据到来
        //if (nTotal >= 0) {
            //如果会话SOCKET有数据到来，则接受客户的数据

            if (FD_ISSET(clntSock, &rfds)) {
                fgets(buf,sizeof(buf),f);//加载文件路径
                //receiving data from client
                memset(rec, '\0', 1024);
                if (recv(clntSock, rec, 1024, 0) > 0) {
                    printf("Received bytes from client:\n %s\n",  rec);
                    FILE *fp;
                    if(rec[5]==' '){
                        string file = string(buf)+"index.html";
                        cout<<file<<endl;
                        fp = fopen(file.c_str(), "rb");
                    }
                    else{
                        char filename[100];
                        int file_num=5;
                        while(rec[file_num]!=' ') {
                            filename[file_num - 5] = rec[file_num];
                            file_num++;
                        }
                        filename[file_num-5]='\0';
                        string file = string(buf)+string(filename);

                        fp = fopen(file.c_str(), "rb");

                    }
                    if(fp== nullptr){
                        send(clntSock,"HTTP/1.1 404 \r\nconnection: close\r\n\r\n",strlen("HTTP/1.1 404 \r\nconnection: close\r\n\r\n"),0);

                    }else{
                        char buf[520] = "HTTP/1.1 200 ok\r\nconnection: close\r\n\r\n";//HTTP响应
                        send(clntSock, buf, strlen(buf), 0);
                        int nCount;
                        char file_buffer[4096] = {0};
                        while ((nCount = fread(file_buffer, 1, 4096, fp)) > 0) {
                            send(clntSock, file_buffer, nCount, 0);
                        }
                    }

                    shutdown(clntSock, SD_SEND);  //文件读取完毕，断开输出流，向客户端发送FIN包
                    fclose(fp);
                }
                else {
                    printf("Client leaving ...\n");
                    closesocket(clntSock);  //既然client离开了，就关闭clntSock
                }

            }
        //}

        //检查会话SOCKET是否有数据到来

    }
}

