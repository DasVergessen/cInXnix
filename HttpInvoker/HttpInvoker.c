#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <sys/types.h>
#include   <sys/socket.h>
#include   <errno.h>
#include   <unistd.h>
#include   <netinet/in.h>
#include   <limits.h>
#include   <netdb.h>
#include   <arpa/inet.h>
#include   <ctype.h>
#include "inc/HttpInvoker.h" 


//////////////////////////////httpclient.c   开始///////////////////////////////////////////
//char * getJsonStr(char * responseBody);

 char * httpGet( char *ipAddr, int port, char * url,char * result)
{
    int sockfd,sendbytes;
    int flag;
    SockaddrIn servAddr;
    
    if(0==strcmp("localhost",ipAddr)){
        strcpy(ipAddr,"127.0.0.1");
    }
    if( (sockfd=inet_addr(ipAddr))==-1 ) {
        perror("Invalid IP Address");
       return result;
    }

/**
 * 0、构造HTTP GET请求
 */
   // char buf[BUFFER_SIZE];
    char * buf = (char*)calloc(BUFFER_SIZE,sizeof(char));
    sprintf(buf,"GET %s HTTP/1.1\r\nhost:%s:%d\r\nAccept:text/html,application/json\r\nAccept-Charset:utf-8,GBK\r\n\r\n",url,ipAddr,port);      
/*1、 socket(int family,int type,int protocol); 　
 * 第一个参数指明了协议簇，目前支持5种协议簇，最常用的有AF_INET(IPv4协议)和AF_INET6(IPv6协议)；
 * 第二个参数指明套接口类型，有三种类型可选：SOCK_STREAM(字节流套接口)、SOCK_DGRAM(数据报套接口)和SOCK_RAW(原始套接口)；
 * 如果套接口类型不是原始套接口，那么第三个参数就为0。
 */    
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket");
       return result;
    }
  //  printf("Establish Socket,flag = %d\n", flag);
/*
 * int inet_pton(int family,const char * strptr,void * addrptr);
 * 返回：1－－－成功　0－－－输入不是有效的表达格式　-1－－－失败 
 * inet_pton函数：将点分十进制串转换成网络字节序二进制值，此函数对IPv4地址和IPv6地址都能处理。
 */    
    inet_pton(AF_INET,ipAddr,&servAddr.sin_addr);
    bzero(&(servAddr.sin_zero),8);
    servAddr.sin_family=AF_INET;
    servAddr.sin_port=htons(port);
/*
 * 2、int connect(int sockfd,const struct sockaddr * servaddr,socklen_t addrlen); 　
 * 当用socket建立了套接口后，可以调用connect为这个套接字指明远程端的地址；
 * 如果是字节流套接口，connect就使用三次握手建立一个连接；
 * 如果是数据报套接口，connect仅指明远程端地址，而不向它发送任何数据。
 * 
 * 第一个参数是socket函数返回的套接口描述字；
 * 第二和第三个参数分别是一个指向套接口地址结构的指针和该结构的大小。
 */   
    if((flag=connect(sockfd,(SockAddr *)&servAddr,sizeof(SockAddr)))==-1){
        //printf("connect error!!! flag = %d\n", flag);
        perror("connect");
        return result;
    }
/*
 *3、int send(SOCKET s,const char FAR *buf,int len,int flags); 
 * 
 * @param argc
 * @param argv
 * @return 
 */    
    if((sendbytes=send(sockfd,buf,strlen(buf),0))==-1){
        perror("send");
        return result;
    }  
    free(buf);
/*
 *4、int recv(SOCKET s,const char FAR *buf,int len,int flags); 
 * 
 * @param argc
 * @param argv
 * @return 
 */   
   //  char *bbb=(char*)calloc(BUFFER_SIZE,sizeof(char));    
    if((sendbytes=recv(sockfd,result,BUFFER_SIZE,0))==-1){
        perror("recv");
        return result;
    }
   //printf("recv%d:\n%s\n",sendbytes,result);
/*
 * 5、CLOSE Socket
 */    
    close(sockfd);
   // exit(0);

  //  char * jsonStr=getJsonStr(bbb);
  //  free(bbb);

    return result;
}
 int getJsonSize(char * responseBody){
    char * strAddr = strstr(responseBody,"{\"");
    char * endAddr = strrchr(responseBody,'}');    
    if(endAddr==strAddr){
        return 0;
    }
    return (endAddr+sizeof(char)-strAddr)/sizeof(char);
 }
 char * getJsonStr(char * responseBody,const int jsonSize,char * out){
    char * strAddr = strstr(responseBody,"{\"");
    strncpy(out,strAddr,jsonSize);
    return out;
}
 
 
//////////////////////////////httpclient.c   结束///////////////////////////////////
