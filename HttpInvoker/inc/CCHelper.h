/* 
 * File:   CCHelper.h
 * Author: hzhang
 *
 * Created on 2014年9月21日, 上午12:30
 */

#ifndef CCHELPER_H
#define	CCHELPER_H

#ifdef	__cplusplus
extern "C" {
#endif

//定义Properties文件中的属性名称   
#define _PART_JSON_FILE 	"part_json_file"
#define _IDA_JSON_FILE 		"ida_json_file"    
#define _IDF_JSON_FILE 		"idf_json_file"    
    
#define _TARGET_PART  		"target_part"
#define _SOURCE_PART  		"source_part"
#define _SOURCE_FILE_NAME  	"source_file_name"
#define _TARGET_FILE_NAME  	"target_file_name"  
    
#define _CC_HOST 		"cc.host"       
#define _CC_PORT		"cc.port"
#define _IDA_URL 		"ida_url"
#define _IDA_URL_SRL 		"ida_url_srl"    
    
#define _IDF_URL 		"idf_url"  
#define _IDF_URL_VER 		"idf_url_ver"    
    
   // extern char * getPart(char * ip, char * ccHost, char * part);
   // extern char * getIDA(char * ip, char * ccHost, char * ida);
   // extern char * getByKey(char * ccHost,char * code,char * key,char *value);
    extern char * getIDA(char * idf,char * ida);
   // extern char * getByKey(char * code,char * key,char * value);
    extern char * getSourceFileName(char * code,char * value);
    extern char * getSourcePart(char * code,char * value);
    extern char * getTargetFileName(char * code,char * value);
    extern char * getTargetPart(char * code,char * value);

#ifdef	__cplusplus
}
#endif

#endif	/* CCHELPER_H */

