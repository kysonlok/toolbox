#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdlib.h>
#include <time.h>

/*
extern int use_syslog;
extern int use_tty;
*/
int use_syslog = 0;
int use_tty = 1;

#define TIME_FORMAT "%F %T"

#define USE_SYSLOG(ident)													\
do {																		\
	use_syslog = 1;															\
	openlog(ident, LOG_CONS | LOG_PID, LOG_DAEMON);							\
} while (0)

#define LOGGING_INFO(format, ...)											\
do {																		\
	if (use_syslog) {														\
		syslog(LOG_INFO, "INFO: " format, ##__VA_ARGS__); 					\
	} else {																\
		time_t now = time(NULL);											\
		char timestr[20];													\
		strftime(timestr, sizeof(timestr), TIME_FORMAT, localtime(&now));	\
		if (use_tty) {														\
			fprintf(stderr, "\e[01;32m%s INFO: \e[0m" format, timestr,		\
##__VA_ARGS__);																\
		} else {															\
			fprintf(stderr, "%s INFO: " format, timestr, ##__VA_ARGS__);	\
		}																	\
	}																		\
} while (0)

#define LOGGING_ERR(format, ...)											\
do {																		\
	if (use_syslog) {														\
		syslog(LOG_ERR, "ERROR: " format, ##__VA_ARGS__);					\
	} else {																\
		time_t now = time(NULL);											\
		char timestr[20];													\
		strftime(timestr, sizeof(timestr), TIME_FORMAT, localtime(&now));	\
		if (use_tty) {														\
			fprintf(stderr, "\e[01;35m%s ERROR: \e[0m" format, timestr,		\
##__VA_ARGS__);																\
		} else {															\
			fprintf(stderr, "%s ERROR: " format, timestr, ##__VA_ARGS__);	\
		}																	\
	}																		\
} while (0)

#define LOGGING_FAFAL(format, ...)											\
do {																		\
	LOGGING_ERR(format, ##__VA_ARGS__);										\
	exit(EXIT_FAILURE);														\
} while (0)

#define LOGGING_ASSERT(cond, format, ...)									\
do {																		\
	if (!cond) {															\
		LOGGING_FAFAL("ASSERT(" #cond "): " format, ##__VA_ARGS__);			\
	}																		\
} while (0)

#endif
