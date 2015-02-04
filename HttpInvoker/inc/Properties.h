/* 
 * File:   Properties.h
 * Author: hzhang
 *
 * Created on 2014年9月25日, 下午11:07
 */

#ifndef PROPERTIES_H
#define	PROPERTIES_H

#ifdef	__cplusplus
extern "C" {
#endif

#define PROPERTIES_FILE "./cft.properties"
    
    extern char * trim(char *chr);
    extern char * getProperty(char * key,char * value);


#ifdef	__cplusplus
}
#endif

#endif	/* PROPERTIES_H */

