
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../inc/fjnxAxway.h"

int main(int argc, char* argv[]) {
    if(argc!=2&&argc!=3){
        puts("Usage:queryTest [ida] [state]\n");
        exit(1);
    }
    
    
    SelectFilter psf = {
        argv[1],
        argv[2]
    };

    char  * str = (char*)calloc(BUFFER_SIZE,sizeof(char));
    queryResult(&psf, str);
      
    printf("%s\n",str);
    free(str);

}






