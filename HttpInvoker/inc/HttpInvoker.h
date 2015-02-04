/* 
 * File:   HttpInvoker.h
 * Author: hzhang
 *
 * Created on 2014年9月19日, 下午4:11
 */

#ifndef HTTPINVOKER_H
#define	HTTPINVOKER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define BUFFER_SIZE 4096
typedef struct sockaddr_in  SockaddrIn; 
typedef struct hostent  HoseEnt;
typedef struct in_addr InAddr;
typedef struct sockaddr SockAddr;

extern  int getJsonSize(char * responseBody);
extern  char * httpGet( char *ipAddr, int port, char * url,char * result); 
extern  char * getJsonStr(char * responseBody,const int size,char * out);

#ifdef	__cplusplus
}
#endif

#endif	/* HTTPINVOKER_H */

