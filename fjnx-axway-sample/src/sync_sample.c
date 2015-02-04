/****************************************************************************
*
* TCFTSYN
* CFT Communication and Catalog API sample program using 
* Synchronous Communication media
*
* Copyright (c) Axway 2006-2013 - Axway Transfer CFT 
*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/fjnxAxway.h"

int main(int argc, char* argv[]){
    if(argc!=6){
        puts("Usage:sync_sample send|recv [part] [idf] [filePath] [ida]\n");
        exit(1);
    }
    int iCr ;

    if(0==strcmp("send",argv[1])){
        FileRequest pfr={
            argv[5],
            argv[2],
            argv[3],
            CFT_SEND,
            argv[4]
        };
        cftTransferSync(&pfr);
        printf("%d\n",iCr);
    }else if(0==strcmp("recv",argv[1])){
        FileRequest pfr = {
            argv[5],
            argv[2],
            argv[3],
            CFT_RECV,
            argv[4]
        };
        cftTransferSync(&pfr);
        printf("%d\n",iCr);
    }else{
        puts("Usage:sync_sample send|recv [part] [idf] [filePath] [ida]\n");
        exit(1);
    }



    return 0;
} 



