#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cftapi.h>

#include "../inc/fjnxAxway.h"

int cftutil_transfer(char * TRANSFER_SERVICE,char * cmd) {
	return cftau(TRANSFER_SERVICE, cmd);
}

/*文件传输请求*/
int cftFileRequestHandler(FileRequest * pfr){
	char *cmd = (char*) calloc(1024,sizeof(char));
        int result;
	int isSend=strcmp(pfr->type,CFT_SEND);
	int isRecv=strcmp(pfr->type,CFT_RECV);
	if(isSend==0){
		sprintf(cmd, SEND_FORMAT,pfr->part, pfr->idf,pfr->filePath, pfr->ida);
		result = cftutil_transfer(CFT_SEND,cmd);
			if (result == CAPI_NOERR) {
				printf("\r\nSend command execute successfully.\r\n");
			} else {
				printf("\r\nERROR: %d\r\n", result);
			}
	}
	if(isRecv==0){
		sprintf(cmd, RECV_FORMAT, pfr->part, pfr->idf,pfr->filePath, pfr->ida);
		 result = cftutil_transfer(CFT_RECV,cmd);
			if (result == CAPI_NOERR) {
				printf("\r\nRecv command execute successfully.\r\n");
			} else {
				printf("\r\nERROR: %d\r\n", result);
			}
	}
	free(cmd);
        return result;
}

char * returnPara(char * in,int size,char *out) {
	int length=strlen(in);
	int maxLen=length<size?length:size;
	strncpy(out, in, maxLen);
	return out;
}
/*
	查询传输结果(异步)
	输入参数：查询条件
*/
char * queryResult(SelectFilter * psf,char * result) {
	char *pAPIref;
	int iRc, iCount;

	cftselT tCFTsel;
	cftcatT tCFTcat;

        memset(result,'\0',sizeof(result));
/*打开日志文件，这里不指定具体的文件，就打开默认日志文件*/
        //puts("OPEN catalog...\n");
	iRc = cftai("OPEN", &pAPIref, "");

	if (iRc) {
            sprintf(result, "API cftai _ Verb OPEN _ error: %d\n", iRc);
            return result;
	}
/*引入查询条件*/
        //puts("initialize tCFTsel...\n");
        memset((char *) &tCFTsel, 0, sizeof(cftselT));
        sprintf(tCFTsel.slength, "%ld", (long) sizeof(cftselT));
        sprintf(tCFTsel.clength, "%ld", (long) sizeof(cftcatT));
        returnPara(psf->ida,IDA_SIZE,tCFTsel.ida);
        if(strlen(psf->state)>0){
            strcpy(tCFTsel.state,psf->state);
        }

/*引入查询完毕，开始查询*/  
        //printf("going to SELECT with tCFTsel->ida:%s,and tCFTsel->state:%s...\n",tCFTsel.ida,tCFTsel.state);
	iRc = cftai("SELECT", &pAPIref, (char *) &tCFTsel);
        if(iRc==CAPI_CAT_EMPTY){
            result="Catalog Empty!";
            return result;
        }	
        if (iRc) {
            sprintf(result, "API cftai _ Verb SELECT _ error: %d\n", iRc);
            cftai("CLOSE", &pAPIref, 0);
            return result;
	}
/*开始遍历结果集*/
        iCount = 0;
	while (1) {
            iRc = cftai("NEXT", &pAPIref, (char *) &tCFTcat);
            if (iRc) {
                if (iRc != CAPI_CAT_EOF) {
                    sprintf(result, "API cftai _ Verb NEXT _ error: %d\n", iRc);
                    return result;
                }
                break;
            } else {
                iCount++;
                char * ida = tCFTcat.ida;
                char * idt = tCFTcat.idt;
                char * part = tCFTcat.part;
                char state = tCFTcat.state;
                int intState = (int) state;
                char * rString = (char *)calloc(128,sizeof(char));                
                switch (intState) {
                    case STATE_T:
                        sprintf(rString, RFORMAT, ida,idt,part,state, "Transfer terminated");
                        break;
                    case STATE_X:
                        sprintf(rString, RFORMAT,ida,idt,part,state,"Transfer procedure executed");
                        break;
                    case STATE_D:
                        sprintf(rString, RFORMAT, ida,idt,part,state,"Transfer available, at disposal");
                        break;
                    case STATE_C:
                        sprintf(rString, RFORMAT, ida,idt,part,state,"Transfer current, in process");
                        break;
                    case STATE_H:
                        sprintf(rString, RFORMAT, ida,idt,part,state,"Transfer hold, interrupted");
                        break;
                    case STATE_K:
                        sprintf(rString, RFORMAT, ida,idt,part,state, "Transfer keep, stopped");
                        break;
                    default:
                        sprintf(rString, RFORMAT, ida,idt,part,state, "querying..");
                }
                strcat(result,rString);
                free(rString);
            }
	}
/*关闭日志文件*/
        iRc = cftai("CLOSE", &pAPIref, 0);
        return result;
}

/*异步方式调用*/
char * cftTransferService(FileRequest * pfr,SelectFilter * psf,char * result){
    cftFileRequestHandler(pfr);
    sleep(10);
    return queryResult(psf,result);
}


/*同步方式调用*/
int cftTransferSync(FileRequest * pfr){
    int iCr;
    cftApiInfT cftinfo;
    
/*1_打开同步通讯介质*/
    printf("OPEN \n");
//#ifdef SYS_MVS                   
//    iCr = cftau("COM","C=$CFTCOMS");       
//#else
    iCr = cftau("COM",COM_FILE);   
//#endif
    if (iCr != CAPI_NOERR) {
       switch (iCr) {
            case CAPI_COM_OPEN: 
                printf("Error while Opening Sync. Comm. Media\n");
                exit(1);
                break;
           case APIS_OPEN_SOCKET:
               printf("Open channel failed\n");
               exit(1);
               break;
            default:printf("error %d\n",iCr);
        } 
    } else {
        printf("Opening Sync. Comm. Media correct.\n");
    } 
/*2_执行文件传输*/
    iCr=cftFileRequestHandler(pfr);
    if (iCr == CAPI_NOERR) {
        printf("SEND OK\n");
    }else{
        printf("SEND NOK, iCr=%d\n",iCr);
        iCr = cftau("CLOSEAPI",NULL) ;
        return 1;
    } 
/*3_获取传输信息*/
    printf("GETXINFO\n");
    iCr = cftau("GETXINFO",(char*) &cftinfo);
    if (iCr == CAPI_NOERR) {
        printf("GETXINFO OK.\n");
        printf("  cr=%s\n",  cftinfo.sCr);
        printf("  idtu=%s\n",cftinfo.idtu);
        printf("  idt=%s\n", cftinfo.idt);

    } else {
        printf("GETXINFO NOK, iCr=%d\n",iCr);   
        return 1;
    }


    if(cftinfo.idtu[0]>' ') {
        char buffer[127+1];
        sprintf(buffer,"SELECT='IDTU==\"%s\"'",cftinfo.idtu);
        printf("SWAITCAT %s\n",buffer);
        iCr = cftau("SWAITCAT",buffer);
        if (iCr == CAPI_NOERR) {
            printf("SWAITCAT OK\n");
        }else{
            const char* msg;
            switch(iCr) {
                case APIS_SWAITCAT_FAILED      : msg="SWAITCAT_FAILED";        break;
                case APIS_SWAITCAT_TIMEOUT     : msg="SWAITCAT_TIMEOUT";       break;
                case APIS_SWAITCAT_NFOUND      : msg="SWAITCAT_NFOUND";        break;
                case APIS_SWAITCAT_DELETED     : msg="SWAITCAT_DELETED";       break;
                case APIS_SWAITCAT_PARAM_ERROR : msg="SWAITCAT_PARAM_ERROR";   break;
                case APIS_SWAITCAT_TOO_MANY    : msg="APIS_SWAITCAT_TOO_MANY"; break;
                default:                         msg="UNKNOWN";
            }
            printf("SWAITCAT NOK %s(%d)\n",msg,iCr);
            iCr = cftau("CLOSEAPI",NULL) ;      
            return -1;
        }
    }
/*关闭API*/
    printf("CLOSEAPI...\n");
    iCr = cftau("CLOSEAPI",NULL); 
    if (iCr == CAPI_NOERR) {
        printf("CLOSEAPI OK.\n");
    } else {
        printf("CLOSEAPI NOK, iCr=%d\n",iCr);
    }
    
    return 0;
    
}
