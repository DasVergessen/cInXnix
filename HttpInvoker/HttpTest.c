#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "inc/HttpInvoker.h"

int   main(int   argc,   char   *argv[]){
    if(argc!=1){
            puts("Usage:HttpTest [URL]\n");
            exit(1);
    }
    char * result = (char *)calloc(2048,sizeof(char));

    httpGet("192.111.6.19",6999,argv[0],result);

    printf("%s\n",result);

    free(result);
    return 0;
}
