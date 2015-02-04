#ifndef FJNXAXWAY_H_
#define FJNXAXWAY_H_

/* DEFINE TRANSFER SERVICE CODE for cftau & cftac */
#define     CFT_SEND    "SEND"
#define     CFT_RECV    "RECV"
#define     CFT_HALT    "HALT"
#define     CFT_KEEP    "KEEP"
#define     CFT_START   "START"
#define     CFT_DELETE  "DELETE"
#define     CFT_END     "END"
#define     CFT_SUBMIT  "SUBMIT"
#define     CFT_SHUT    "SHUT"
#define     CFT_SWITCH  "SWITCH"
#define     CFT_COM     "START"

#define  	 SEND_FORMAT "part=%s,idf=%s,fname=%s,ida=%s"
#define  	 RECV_FORMAT "part=%s,idf=%s,nfname=%s,ida=%s"
#define          RFORMAT   "IDA=%s,IDT=%-8.8s,PART=%-8.8s,Return Code is:%c,%s.\n"
#define          IDA_SIZE  9
#define          BUFFER_SIZE 4096
#define          COM_FILE "C=/home/cft/com.cfg"

#define			INT_A					(int)'A'
#define 		STATE_T					INT_A+19
#define			STATE_X					INT_A+23
#define			STATE_D					INT_A+3
#define			STATE_C					INT_A+2
#define			STATE_H					INT_A+7
#define			STATE_K					INT_A+10

typedef struct {
	char * ida;
	char * part;
	char * idf;
	char * type;
	char * filePath;
} FileRequest;

typedef struct {
	char * ida;
	char * state;
} SelectFilter;

extern int cftau(char*, char*);
extern int cftac(char*, char*);
extern int cftai(char *, char **,char*);
extern int cftaix(char *, char **, char*);

extern char * queryResult(SelectFilter * psf,char * result);
/*异步方式调用*/
extern char * cftTransferService(FileRequest * pfr,SelectFilter * psf,char * result);
/*同步方式调用*/
extern int cftTransferSync(FileRequest * pfr);
#endif /* FJNXAXWAY_H_ */
