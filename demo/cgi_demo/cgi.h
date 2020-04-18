#ifndef CGI_LOG_H
#define CGI_LOG_H

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Common 
 */

#define CGI_LTE_DEFAULT_DEV_IDX		1
#define CGI_CFG_REQ_TIMEOUT_LONG		3 /* Seconds */
#define CGI_LTE_REQ_TIMEOUT_LONG		9 /* Seconds */
#define CGI_USB_REQ_TIMEOUT_LONG		20 /* Seconds */
#define CGI_LTE_CB_TIMEOUT_SHORT	   2 /* seconds */
#define CGI_UPGRADE_CHECK_TIMEOUT   120 /* seconds */
#define CGI_PLMN_ACTION_TIMEOUT     120 /* seconds */
#define CGI_POST_BUF_MAX			2048
#define CGI_SYSTEM_CMD_MAX			512
#define CGI_UCFG_BUF_MAX			128
#define CGI_LTE_REQ_BUF_MAX      2048

#define CGI_LOGIN_COOKIE_FILE		"/tmp"

#define CGI_COOKIE_LENGTH_MAX		4096

#define CGI_REQUEST_COMMAND 	"Command"

#define CONF_ITEM_LEN			64
#define FILE_NAME_LEN			256
#define SYSTEM_CMD_LEN			256

#define CGI_XML_BEGIN() 	\
			fprintf(stdout, "Content-type: text/xml\n\n<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<xml>\n" )
#define CGI_XML_VALUE(element, arg...)\
			do {\
				fprintf(stdout, "<%s>", element); \
				printf(arg); \
				fprintf(stdout, "</%s>\n", element); \
			} while(0)
#define CGI_XML_NODE_BEGIN(node) fprintf(stdout, "<%s>\n", node);
#define CGI_XML_NODE_END(node) fprintf(stdout, "</%s>\n", node);
#define CGI_XML_END() \
			fprintf(stdout, "</xml>\n")
			
#define CGI_HTML_BEGIN() \
			fprintf(stdout, "Content-type: text/html\n\n<html><link rel=stylesheet href=./css/gct_new.css><body>\n")
#define CGI_HTML_REFRESH() \
			fprintf(stdout, "Content-type: text/html\n\n<html><body onload=\"setTimeout('history.go(-1)', 3000)\"></body><html>")
#define CGI_HTML_END() \
			fprintf(stdout, "</body></html>\n")

#define CGI_APP_BEGIN() \
			fprintf(stdout, "Content-type: application/octet-stream\n\n");
#define CGI_APP_END() 

#define CGI_HTML_BODY(arg...) do { printf(arg); fprintf(stdout, "<br>"); } while(0)

#define CGI_NULL_RETURN(pointer, required) \
			do { \
				if (pointer == NULL) \
				{ \
					CGI_XML_VALUE("Error", "Required field \"%s\" is missing", required); \
					return -1; \
				} \
			} while(0)

/*
** System Info
*/
#define PROJECT_DEVICE_NAME "GDM5215 Reference"
#define PROJECT_DEVICE_VERSION "0.0"
#define PROJECT_FIRMWARE_VERSION "0.0"

/*
 * Inline util functions
 */

#define MAC_DEFAULT_HEX			"000000000000"
#define MAC_DEFAULT_STRING		"00:00:00:00:00:00"

static int cgi_log_enabled = 0;

static inline int mac_to_hex(const char *mac, char *hex)
{
	int mac0, mac1, mac2, mac3, mac4, mac5;
	sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", &mac0, &mac1, &mac2, &mac3, &mac4, &mac5);
	sprintf(hex, "%02x%02x%02x%02x%02x%02x", mac0, mac1, mac2, mac3, mac4, mac5);
	hex[strlen(MAC_DEFAULT_HEX)] = '\0';
	return 0;
}

static inline int hex_to_mac(char *hex, char *mac)
{
	int mac0, mac1, mac2, mac3, mac4, mac5;
	sscanf(hex, "%02x%02x%02x%02x%02x%02x", &mac0, &mac1, &mac2, &mac3, &mac4, &mac5);
	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", mac0, mac1, mac2, mac3, mac4, mac5);
	mac[strlen(MAC_DEFAULT_STRING)] = '\0';
	return 0;
}

/*
 * Logs
 */

#define DEBUG

#if defined(DEBUG)

static inline void getLogTime(long int *sec, long int *usec)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*sec = tv.tv_sec;
	*usec = tv.tv_usec;
}

#define cgi_log(arg...) \
	do { \
		if(cgi_log_enabled) {	\
			long sec, usec; \
			getLogTime(&sec, &usec); \
			fprintf(stderr, "[PID:%05d][%ld.%ld][%s][%s] ", getpid(), sec, usec, __FILE__, __FUNCTION__); \
			fprintf(stderr, arg); \
		}	\
	} while (0)

#else

#define cgi_log(arg...)

#endif

static inline int hex_digit_to_int(char hex)
{
	int val = hex - '0';
	return val;
}

#endif /* CGI_LOG_H */

