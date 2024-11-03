/*
 * Log.h
 *
 *  Created on: 2015-11-6
 *      Author: Administrator
 */

#ifndef LOG_H_
#define LOG_H_
#include <stdio.h>

#define _LOG_ON_
//#define _TRACE_ON_

#ifdef __cplusplus
extern "C"{
#endif
void log_init(const char* szFile);
void log_unInit();
void log_write(const char* format, ...);
#ifdef __cplusplus
}
#endif

#ifdef _LOG_ON_
#define LOG_INIT(file) log_init(file)
#define LOG_UNINIT() log_unInit()
#define LOG(...) log_write(__VA_ARGS__)
#else
#define LOG_INIT(file)
#define LOG_UNINIT()
#define LOG(...)
#endif

#ifdef _TRACE_ON_
#define TRACE(...) LOG(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#endif /* LOG_H_ */
