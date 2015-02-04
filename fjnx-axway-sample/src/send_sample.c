
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../inc/fjnxAxway.h"

int main(int argc, char* argv[]) {
    if(argc!=5){
        puts("Usage:send_sample [part] [idf] [filePath] [ida]\n");
        exit(1);
    }
    
    FileRequest pfr = {
        argv[4],
        argv[1],
        argv[2],
        CFT_SEND,
        argv[3]
    };

    SelectFilter psf = {
        pfr.ida,
        "X"
    };

    char  * str = (char*)calloc(BUFFER_SIZE,sizeof(char));
    cftTransferService(&pfr,&psf,str);
      
    printf("%s\n",str);
    free(str);

}





