#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "inc/Properties.h"

char * trim(char *str){
    char *p = str;
    char *p1;
    if(p){
        p1 = p + strlen(str) - 1;
        while(p && isspace(*p)) {
            p++;
        }
        while(p1 > p && isspace(*p1)) {
            *p1-- = '\0';
        }
    }
    return p;
}

char * getProperty(char * key,char * value){
 /*
 1、打开文件
 */
    FILE * fp=fopen(PROPERTIES_FILE,"r+");
    if(fp==NULL){
        return value;
    }
/*
 3、读取及同步内容
 */
    if(fp!=NULL){
        char * line = (char*)calloc(512,sizeof(char));
        char * tmpValue = (char*)calloc(512,sizeof(char));
        while(fgets(line,512,fp)!=NULL){
            //strcat(localContent,line);   
            char * content=trim(line);
            if(strchr(content,'#')!=content&&strstr(content,key)==content){
                char * valueHead = strchr(content,'=');
                char * valueTail = strchr(content,'#');
                int size=0;
                int sizeOfContent=strlen(content);
                
                if(valueTail!=NULL){
                    size = valueTail-valueHead-1;
                }
                int length=(size==0)?(content+sizeOfContent-valueHead-1):size;
                strncpy(tmpValue,valueHead+1,length);
                //trim(tmpValue);
                strcpy(value,trim(tmpValue));
               // printf("%s\n",content);
                break;
            }
        }
        free(tmpValue);
        free(line);
    }
    
     fclose(fp);
     
     return value;
}