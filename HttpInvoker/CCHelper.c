#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>

#include "inc/CCHelper.h"
#include "inc/HttpInvoker.h"
#include "inc/cJSON.h"
#include "inc/Properties.h"

//static char PART_JSON_FILE[64]; 
static char IDA_JSON_FILE[64];  
static char IDF_JSON_FILE[64];    
    
static char TARGET_PART[64];  
static char SOURCE_PART[64];  
static char SOURCE_FILE_NAME[128]; 
static char TARGET_FILE_NAME[128];  
    
static char CC_HOST[32];
static char CC_PORT[8];
static char IDA_URL[256]; //"/ida/get.html?idf=%s"
static char IDA_URL_SRL[256];// "/ida/get.html?idf=%s&serialNumber=%s"    
    
static char IDF_URL[256]; //"/idf/sync.html"  
static char IDF_URL_VER[256]; //"/idf/sync.html?version=%s" 

void init(){
    getProperty(_IDA_JSON_FILE,IDA_JSON_FILE);
    getProperty(_IDF_JSON_FILE,IDF_JSON_FILE);
    
    getProperty(_TARGET_PART,TARGET_PART);
    getProperty(_SOURCE_PART,SOURCE_PART);
    getProperty(_SOURCE_FILE_NAME,SOURCE_FILE_NAME);
    getProperty(_TARGET_FILE_NAME,TARGET_FILE_NAME);

    getProperty(_CC_HOST,CC_HOST);
    getProperty(_CC_PORT,CC_PORT);
    getProperty(_IDA_URL,IDA_URL);
    getProperty(_IDA_URL_SRL,IDA_URL_SRL);
    getProperty(_IDF_URL,IDF_URL);
    getProperty(_IDF_URL_VER,IDF_URL_VER);
}

int parseIPandPort(char * host,char * ip){
    char * colonAddr = strrchr(host,':');
    if(colonAddr>0){
        int size=(colonAddr-host)/sizeof(char);
        strncpy(ip,host,size);
        char chrPort[5] ={};
        strcpy(chrPort,colonAddr+sizeof(char));
        return atoi(chrPort);
    }else{
        strcpy(ip,host);
        return 80;
    }        
}

char * httpGetJSon(char * ccHost,char * url,char * out){
    char * result = (char *)calloc(BUFFER_SIZE,sizeof(char));
    char * hostIP = (char*)calloc(15,sizeof(char));
    int hostPort;
    hostPort=parseIPandPort(ccHost,hostIP);
    // printf("%s:%d\n",hostIP,hostPort);
    httpGet(hostIP,hostPort,url,result);
    if(strlen(result)>0){
        int size = getJsonSize(result);
        if(size>0){
            char * jsonStr=(char*)calloc(size,sizeof(char));
            getJsonStr(result,size,jsonStr);    
            //printf("%s\n",jsonStr);
            strcpy(out,jsonStr);
            free(jsonStr);
        }
    }
    free(hostIP);
    free(result);
    return out;
}

char * trimQuotes(char * in){
    int length=strlen(in);
    if(length>0){
        char * targetValue = (char*)calloc(length,sizeof(char));
        strcpy(targetValue,in);
        char * head=strchr(targetValue,'\"');        
        char * end=strrchr(targetValue,'\"');
        if(head==targetValue&&(targetValue+length-1)==end){
            memset(in,'\0',length);
            strncpy(in,targetValue+1,length-2);
        }
        free(targetValue);
    }
    return in;
}

char * getSerialNumber(char * prefix,char * serialNumber,int length,char *out){
    char chars[length];
    int i=0;
    for(i=0;i<length;i++){
        chars[i]='0';
    }
    int prefixLength=strlen(prefix);
    if(prefixLength>0){
        char prefixArr[prefixLength];
        strcpy(prefixArr,prefix);
        for(i=0;i<prefixLength;i++){
            chars[i] = prefixArr[i];
        }
    }
    int serialLength =strlen(serialNumber);
    if(serialLength>0){
        int index=1;
        char endArray[serialLength];
        strcpy(endArray,serialNumber);
        for(i=serialLength-1;i>=0;i--){
            chars[length-index++]=endArray[i];
        }
    }
    strcpy(out,chars);
    return out;
}

char * copyStr(char * localContent,char * httpResult){
    if(strlen(httpResult)>0){
        cJSON * syncJson=cJSON_Parse(httpResult);
        
        if(syncJson&&(0==strcmp("true",cJSON_Print(cJSON_GetObjectItem(syncJson,"success"))))){    
            cJSON * syncJsonContent = cJSON_GetObjectItem(syncJson,"content");
            memset(localContent,'\0',sizeof(localContent));
            strcpy(localContent,cJSON_Print(syncJsonContent));
        }
        cJSON_Delete(syncJson);
    }
    return localContent;
}

char * syncIdfByHttp(char * ccHost,char * localContent){
        char * httpResult = (char *)calloc(BUFFER_SIZE,sizeof(char));
        char * url = (char*)calloc(256,sizeof(char));
        char * versionInLocal = (char*)calloc(32,sizeof(char));
	if(strlen(localContent)==0){   //本地文件为空，尝试发起http同步请求，
            strcpy(url,IDF_URL);
            httpGetJSon(ccHost,url,httpResult);
            //printf("resync:%s\n",httpResult);
            copyStr(localContent,httpResult);          
        }else{                      //本地文件不为空,则与服务器比较版本差异
            cJSON * localJson=cJSON_Parse(localContent);
            if(localJson){
                // cJSON * localJsonContent = cJSON_GetObjectItem(localJson,"content");
                 strcpy(versionInLocal,cJSON_Print(cJSON_GetObjectItem(localJson,"version")));
                 cJSON_Delete(localJson);
                 //printf("%s\n"versionInLocal);
                 sprintf(url,IDF_URL_VER,versionInLocal);
                 httpGetJSon(ccHost,url,httpResult);
                 //printf("resync 2:%s\n",httpResult);
                 copyStr(localContent,httpResult);    
            }
        }        
        free(versionInLocal);
        free(url);
        free(httpResult);
        return localContent;
}

void getIDAbyHttp(char * ccHost,char * idf,char * specialCode,char * serialNumber,char * ida){
//尝试从远程服务器获得ida
    char * resultJson = (char*)calloc(BUFFER_SIZE,sizeof(char));
    char * url = (char*)calloc(256,sizeof(char));
    if(0==strlen(serialNumber)){
        sprintf(url,IDA_URL,idf);
    }else{
        sprintf(url,IDA_URL_SRL,idf,serialNumber);
    }
    httpGetJSon(ccHost,url,resultJson);
//获取远程服务器上的ida,specialCode,serialNumber
    //if(strlen(resultJson)>0){
    cJSON * json = cJSON_Parse(resultJson);
    if(json){
        if(0==strcmp("true",cJSON_Print(cJSON_GetObjectItem(json,"success")))){
            cJSON * contentJson = cJSON_GetObjectItem(json,"content");
            strcpy(ida,cJSON_Print(cJSON_GetObjectItem(contentJson,"code")));
            //printf("http ida:%s\n",ida);
            strcpy(specialCode,cJSON_Print(cJSON_GetObjectItem(contentJson,"specialCode")));
            char * srlNbrInHttp = cJSON_Print(cJSON_GetObjectItem(contentJson,"serialNumber"));
            if(0==strlen(serialNumber)||atol(serialNumber)<atol(srlNbrInHttp)){
                sprintf(serialNumber,"%s",srlNbrInHttp);
            }
        }
        cJSON_Delete(json);
        trimQuotes(ida);
        trimQuotes(specialCode);
        trimQuotes(serialNumber);        
    }
    free(url);
    free(resultJson);
}

char * getValueByKey(char * code, char * key,char * value,char * localContent){
    char * idf = (char*)calloc(strlen(code)+2,sizeof(char));
   // char * targetValue=(char*)calloc(256,sizeof(char));
   // printf("localContent:%s\n",localContent);
    cJSON * contentJson = cJSON_Parse(localContent);
    if(contentJson){
    //  printf("contents:%s\n",cJSON_Print(contentJson));
      cJSON * listJson = cJSON_GetObjectItem(contentJson,"list");
    //  printf("list:%s\n",cJSON_Print(listJson));
      int jsonArrSize=cJSON_GetArraySize(listJson);
      int i=jsonArrSize;
      for(i=jsonArrSize;i>0;i--){
          cJSON * partJson = cJSON_GetArrayItem(listJson, i);
          sprintf(idf,"\"%s\"",code);
          char * itemIdf=cJSON_Print(cJSON_GetObjectItem(partJson,"code"));
          if(0==strcmp(idf,itemIdf)){
              strcpy(value,cJSON_Print(cJSON_GetObjectItem(partJson,key)));
              break;
          }
      }
  /*
      int length=strlen(targetValue);
      //printf("value:%s:%d\n",targetValue,length);
      if(length>0){
          char * head=strchr(targetValue,'\"');        
          char * end=strrchr(targetValue,'\"');
          if(head==targetValue&&(targetValue+length-1)==end){
              strncpy(value,targetValue+1,length-2);
          }
      }
  */
      cJSON_Delete(contentJson);
    }
    // free(targetValue);
    trimQuotes(value);
    free(idf);
    return value;
}

char * updateLocalJson(char * idf,char * specialCode,char * serialNumber,char * localContent){
    cJSON * localJson = cJSON_Parse(localContent);
//更新或追加本地jSon的内容           
    if(localJson){
        cJSON * listJson = cJSON_GetObjectItem(localJson,"list");
        //printf("Before:%s\n",cJSON_PrintUnformatted(localJson));
        char * tmpIdf = (char*)calloc(strlen(idf)+2,sizeof(char));
        sprintf(tmpIdf,"\"%s\"",idf);
        int jsonArrSize=cJSON_GetArraySize(listJson);
        int i=0;
        //short exist=0;
        for(i=1;i<=jsonArrSize;i++){
            cJSON * itemJson = cJSON_GetArrayItem(listJson, i);
            char * itemIdf = cJSON_Print(cJSON_GetObjectItem(itemJson,"idf"));
            if(0==strcmp(tmpIdf,itemIdf)){
                cJSON_DeleteItemFromArray(listJson, i-1);
                break;
            }
        }
        //printf("After Delete:%s\n",cJSON_PrintUnformatted(localJson));

        cJSON * new=cJSON_CreateObject();	
        cJSON_AddStringToObject(new,"specialCode",specialCode);
        cJSON_AddStringToObject(new,"idf",idf);
        cJSON_AddNumberToObject(new,"serialNumber",atol(serialNumber));        
        cJSON_AddItemToArray(listJson,new);

        //printf("After Add:%s\n",cJSON_PrintUnformatted(localJson));

       
        free(tmpIdf);

    }else{//本地内容为空或非json格式，就重构本地的jSon内容
        cJSON *list,*node;
        localJson=cJSON_CreateObject();	
        list=cJSON_CreateArray();
        node=cJSON_CreateObject();	
        cJSON_AddStringToObject(node,"specialCode",specialCode);
        cJSON_AddStringToObject(node,"idf",idf);
        cJSON_AddNumberToObject(node,"serialNumber",atol(serialNumber));
        cJSON_AddItemToArray(list,node);         
        cJSON_AddItemToObject(localJson,"list",list);
        //printf("After Add:%s\n",cJSON_PrintUnformatted(localJson));
    }       
    if(localJson){
        memset(localContent,'\0',strlen(localContent));
        strcpy(localContent,cJSON_Print(localJson));
        cJSON_Delete(localJson);
    }
    return localContent;
}

char * getIDALocal(char * host,char * port,char * idf,char * ida){
    //先读取本地内容的specialCode及serialNumber
    char * ccHost = (char*)calloc(24,sizeof(char));
    char * contentBuf = (char*)calloc(BUFFER_SIZE,sizeof(char));
    char * specialCode = (char*)calloc(32,sizeof(char));
    char * serialNumber = (char*)calloc(13,sizeof(char));
  /*
 1、打开文件
 */
    FILE * fp=fopen(IDA_JSON_FILE,"r+");
    if(fp==NULL){
        fp=fopen(IDA_JSON_FILE,"w+");
    }
    //printf("file %s open...\n",IDA_JSON_FILE);
/*
 2、文件加锁
 */
    //printf("trying to lock file %s...\n",IDA_JSON_FILE);
    flock(fileno(fp),LOCK_EX);
    //printf("file %s has been locked...\n",IDA_JSON_FILE);
    //sleep(15);
/*
 3、读取文件
 */
    if(fp!=NULL){
        char * line = (char*)calloc(512,sizeof(char));
        while(fgets(line,512,fp)!=NULL){
            strcat(contentBuf,line);
        }
        free(line);
    }
    //printf("local:%s\n",contentBuf);
    if(strlen(contentBuf)>0){
        char * tmpIdf = (char*)calloc(strlen(idf)+2,sizeof(char));
        sprintf(tmpIdf,"\"%s\"",idf);
        cJSON * contentJson = cJSON_Parse(contentBuf);
        if(contentJson){
            cJSON * listJson = cJSON_GetObjectItem(contentJson,"list");
            int jsonArrSize=cJSON_GetArraySize(listJson);
            int i=jsonArrSize;
            for(i=jsonArrSize;i>0;i--){
                cJSON * itemJson = cJSON_GetArrayItem(listJson, i);
                char * itemIdf = cJSON_Print(cJSON_GetObjectItem(itemJson,"idf"));
                if(0==strcmp(tmpIdf,itemIdf)){
                    strcpy(specialCode,cJSON_Print(cJSON_GetObjectItem(itemJson,"specialCode")));
                    strcpy(serialNumber,cJSON_Print(cJSON_GetObjectItem(itemJson,"serialNumber")));
                    trimQuotes(specialCode);
                    trimQuotes(serialNumber);
                    break;
                }

            //printf("%s:%s\n",specialCode,serialNumber);        
            }
            cJSON_Delete(contentJson);
        }
        free(tmpIdf);
    }
//尝试从远程读取specialCode,serialNumber,ida
    sprintf(ccHost,"%s:%s",host,strlen(port)==0?"80":port);
    getIDAbyHttp(ccHost,idf,specialCode,serialNumber,ida);
    
//如无法通过网络获得ida，就根据本地获取的specialCode+serialNumber计算出ida
    if (0==strlen(ida)&&strlen(serialNumber)>0&&strlen(specialCode)>0) {
        long nextSerial = atol(serialNumber)+1;
        sprintf(serialNumber,"%d",nextSerial);
        getSerialNumber(specialCode, serialNumber, 13,ida);
    }
/*
  4、写入本地文件
*/
    if(strlen(serialNumber)>0&&strlen(specialCode)>0){
        updateLocalJson(idf,specialCode,serialNumber,contentBuf);
        ftruncate(fileno(fp),0);
        rewind(fp);
       // char * writeJson = (char*)calloc(256,sizeof(char));
        //sprintf(writeJson,"{\"specialCode\":\"%s\",\"serialNumber\":%s,\"code\":\"%s\"}",specialCode,serialNumber,ida);
        //writeLocalContent(IDA_JSON_FILE,writeJson);
        fputs(contentBuf,fp);
       // fputs("\n",fp);
       // free(writeJson);
    }
/*
 5、解锁，关闭文件
 */
    flock(fileno(fp),LOCK_UN);
    //printf("file %s has been unlocked...\n",IDA_JSON_FILE);
    fclose(fp);
    //printf("file %s closed...\n",IDA_JSON_FILE);
    
    free(specialCode);
    free(serialNumber);
    free(contentBuf);
    free(ccHost);
    return ida;
}

char * getByKey(char * host,char * port,char * code,char * key,char * value){
    char * ccHost = (char*)calloc(32,sizeof(char));
    char * versionInHttpResult = (char*)calloc(32,sizeof(char));
    char * localContent = (char*)calloc(BUFFER_SIZE,sizeof(char));
/*
 1、打开文件
 */
    FILE * fp=fopen(IDF_JSON_FILE,"r+");
    if(fp==NULL){
        fp=fopen(IDF_JSON_FILE,"w+");
    }
    //printf("file %s open...\n",IDF_JSON_FILE);
/*
 2、文件加锁（exclusive lock)
 */
    //printf("trying to lock file %s...\n",IDF_JSON_FILE);
    flock(fileno(fp),LOCK_EX);
    //printf("file %s has been locked...\n",IDF_JSON_FILE);
    //sleep(15);
/*
 3、读取及同步内容
 */
    if(fp!=NULL){
        char * line = (char*)calloc(512,sizeof(char));
        while(fgets(line,512,fp)!=NULL){
            strcat(localContent,line);
        }
        free(line);
    }
    //printf("local:%s\n",localContent);
    sprintf(ccHost,"%s:%s",host,strlen(port)==0?"80":port);
    syncIdfByHttp(ccHost,localContent);
    
/*
    4、写入本地文件
*/
    if(strlen(localContent)>0){
        //writeLocalContent(PART_JSON_FILE,httpResult);
        getValueByKey(code,key,value,localContent);
        ftruncate(fileno(fp),0);
        rewind(fp);
        fputs(localContent,fp);
        fputs("\n",fp);
    }
/*
 5、解锁，关闭文件
 */
    flock(fileno(fp),LOCK_UN);
    //printf("file %s has been unlocked...\n",IDF_JSON_FILE);
    fclose(fp);
    //printf("file %s closed...\n",IDF_JSON_FILE);
    
    free(localContent);
    free(versionInHttpResult);
    free(ccHost);
    return value;
}

char * getIDA(char * idf,char * ida){
    if(strlen(CC_HOST)==0) init();
    return getIDALocal(CC_HOST,CC_PORT,idf,ida);
}

char * getSourceFileName(char * code,char * value){
    if(strlen(CC_HOST)==0) init();
    return getByKey(CC_HOST,CC_PORT,code,SOURCE_FILE_NAME,value);
}

char * getSourcePart(char * code,char * value){
    if(strlen(CC_HOST)==0) init();
    return getByKey(CC_HOST,CC_PORT,code,SOURCE_PART,value);
}

char * getTargetFileName(char * code,char * value){
    if(strlen(CC_HOST)==0) init();
    return getByKey(CC_HOST,CC_PORT,code,TARGET_FILE_NAME,value);
}

char * getTargetPart(char * code,char * value){
    if(strlen(CC_HOST)==0) init();
    return getByKey(CC_HOST,CC_PORT,code,TARGET_PART,value);
}