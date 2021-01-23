#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#define BUF_SIZE 4096
using namespace std;

const char SERVER_IP[] = "127.0.0.1";

int main(int argc, char *argv[])
{
    //Create socket
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("could not create socket");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(8888);

    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        exit(1);
    }

    //keep communicating with server
    int count = 0;
  
       string msg = "GET / HTTP/1.1 \r\nHost: 192.168.174.136:8081\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:84.0) Gecko/20100101 Firefox/84.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nConnection: close\r\nUpgrade-Insecure-Requests: 1\r\nIf-Modified-Since: Fri, 22 Jan 2021 22:40:59 GMT";
       //cin>>msg;

        //Send count

        if (send(sock, msg.c_str(), msg.length(), 0) < 0)
        {
            perror("send failed");
            exit(1);
        }

        char message[BUF_SIZE];
        //Receive a reply from the server
        if (recv(sock, message, BUF_SIZE - 1, 0) < 0)
        {
            perror("recv failed");
            exit(1);
        }
        else
        {
            cout << message << endl;
        }
    close(sock);
    exit(0);
}
