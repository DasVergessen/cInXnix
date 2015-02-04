#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "inc/CCHelper.h"

int   main(int   argc,   char   *argv[]){
    
    char * out=(char*)calloc(32,sizeof(char));
    //printf("%s,%s",argv[1],argv[2]);
    if(argc!=3)  {
        printf("usage: HttpInvoker [IDA|SourceFileName|SourcePart|TargetFileName|TargetPart] [key|idf]\n");
        exit(1);
    }
    if(0==strcmp("IDA",argv[1])){
        getIDA(argv[2],out);
    }
    else if(0==strcmp("SourceFileName",argv[1])){
        getSourceFileName(argv[2],out);
    }
    else if(0==strcmp("SourcePart",argv[1])){
        getSourcePart(argv[2],out);
    }
    else if(0==strcmp("TargetFileName",argv[1])){
        getTargetFileName(argv[2],out);
    }
    else if(0==strcmp("TargetPart",argv[1])){
        getTargetPart(argv[2],out);
    }else{
        strcpy(out,"usage: [IDA|SourceFileName|SourcePart|TargetFileName|TargetPart] [key|idf]\n");
    }
    printf("%s\n",out);
    free(out);
    return 0;
}


